//
// Created by dburc on 11/27/2024.
//

#include "SliceHeader.h"
#include "ByteStream.h"
#include "ExpGolomb.h"
#include "BufferStream.h"
#include <iostream>

#define IN_RANGE_OR_RETURN(val, min, max)                                   \
  do {                                                                      \
    if ((val) < (min) || (val) > (max)) {                                   \
      std::cout << "Error in stream: invalid value, expected " #val " to be"\
               << " in range [" << (min) << ":" << (max) << "]"             \
               << " found " << (val) << " instead\n";                       \
      return false;                                                         \
    }                                                                       \
  } while (0)

namespace h26x {
    bool SliceHeader::isPSlice() const {
        return (slice_type % 5 == 0);
    }

    bool SliceHeader::isBSlice() const {
        return (slice_type % 5 == 1);
    }
    bool SliceHeader::isISlice() const {
        return (slice_type % 5 == 2);
    }

    bool SliceHeader::isSPSlice() const {
        return (slice_type % 5 == 3);
    }

    bool SliceHeader::isSISlice() const {
        return (slice_type % 5 == 4);
    }

    bool SliceHeader::parseRefPicListModification(BitStream * br,
                                                     uint32_t num_ref_idx_active_minus1,
                                                     std::vector<H264ModificationOfPicNum> *ref_list_mods) {
        const auto s = num_ref_idx_active_minus1 + 1;
        ref_list_mods->reserve(s);
        for (int i = 0; s; ++i) {
            auto modification_of_pic_nums_idc = ExpGolomb::get(br);
            if (modification_of_pic_nums_idc >= 4) {
                return false;
            }

            switch (modification_of_pic_nums_idc) {
                case 0:
                case 1:
                    // abs_diff_pic_num_minus1
                case 2:
                    // long_term_pic_num
                    ref_list_mods->emplace_back(H264ModificationOfPicNum{modification_of_pic_nums_idc, ExpGolomb::get(br)});
                    break;

                case 3:
                    // Per spec, list cannot be empty.
                    if (i == 0)
                        return false;
                    return true;

                default:
                    //return kInvalidStream;
                    break;
            }
        }
        return false;
    }

    bool SliceHeader::parseRefPicListModifications(BitStream * br) {
        if (!isISlice() && !isSISlice()) {
            ref_pic_list_modification_flag_l0 = br->get();
            if (ref_pic_list_modification_flag_l0) {
                bool res = parseRefPicListModification(br, num_ref_idx_l0_active_minus1,
                                                       &ref_list_l0_modifications);
                if (!res) {
                    return res;
                }
            }
        }

        if (isBSlice()) {
            ref_pic_list_modification_flag_l1 = br->get();
            if (ref_pic_list_modification_flag_l1) {
                return parseRefPicListModification(br, num_ref_idx_l1_active_minus1,
                                                   &ref_list_l1_modifications);
            }
        }
        return true;
    }
    bool SliceHeader::parseWeightingFactors(
            BitStream * br,
            uint32_t num_ref_idx_active_minus1,
            uint8_t chroma_array_type,
            H264WeightingFactors* w_facts) {
        auto s = num_ref_idx_active_minus1 + 1;

        w_facts->luma_weight = new int8_t[s];
        std::fill_n(w_facts->luma_weight, s, getLumaWeightDenom());
        w_facts->luma_offset = new int8_t[s]{};
        if (chroma_array_type != 0) {
            w_facts->chroma_weight = new int8_t[s][2];
            std::fill_n(&w_facts->chroma_weight[0][0], s*2, getChromaWeightDenom());
            w_facts->chroma_offset = new int8_t[s][2]{};
        }

        for (int i = 0; i < s; ++i) {
            //luma_weight_flag
            if (br->get()) {
                auto v = ExpGolomb::getSigned(br);
                IN_RANGE_OR_RETURN(v, -128, 127);
                w_facts->luma_weight[i] = (int8_t)v;

                v = ExpGolomb::getSigned(br);
                IN_RANGE_OR_RETURN(v, -128, 127);
                w_facts->luma_offset[i] =(int8_t)v;
            }

            if (chroma_array_type != 0) {
                //chroma_weight_flag
                if (br->get()) {
                    for (int j = 0; j < 2; ++j) {
                        auto v = ExpGolomb::getSigned(br);
                        IN_RANGE_OR_RETURN(v, -128, 127);
                        w_facts->chroma_weight[i][j] = (int8_t)v;

                        v = ExpGolomb::getSigned(br);
                        IN_RANGE_OR_RETURN(v, -128, 127);
                        w_facts->chroma_offset[i][j] = (int8_t)v;
                    }
                }
            }
        }

        return br->isOk();
    }

    bool SliceHeader::parsePredWeightTable(BitStream * br, SPS * sps) {
        luma_log2_weight_denom = ExpGolomb::get(br);
        if (luma_log2_weight_denom >= 8) {
            return false;
        }

        if (sps->getChromaArrayType() != 0) {
            chroma_log2_weight_denom = ExpGolomb::get(br);
        }
        if (chroma_log2_weight_denom >= 8) {
            return false;
        }
        pred_weight_table_l0 = std::make_unique<H264WeightingFactors>();
        auto res = parseWeightingFactors(br,
                num_ref_idx_l0_active_minus1, sps->getChromaArrayType(),
                pred_weight_table_l0.get());
        if (!res) return res;

        if (isBSlice()) {
            pred_weight_table_l1 = std::make_unique<H264WeightingFactors>();
            res = parseWeightingFactors(br,
                    num_ref_idx_l1_active_minus1, sps->getChromaArrayType(),
                    pred_weight_table_l1.get());
            if (!res) return res;
        }

        return br->isOk();

    }
    bool SliceHeader::parseDecRefPicMarking(BitStream * br, bool idr) {
        if (idr) {
            no_output_of_prior_pics_flag = br->get();
            long_term_reference_flag = br->get();
        } else {
            adaptive_ref_pic_marking_mode_flag = br->get();

            if (adaptive_ref_pic_marking_mode_flag) {

                uint32_t memory_mgmnt_control_operation;
                while ((memory_mgmnt_control_operation = ExpGolomb::get(br)) != 0) {
                    ref_pic_marking.emplace_back();
                    auto marking = ref_pic_marking.back();
                    marking.memory_mgmnt_control_operation = memory_mgmnt_control_operation;
                    if (marking.memory_mgmnt_control_operation == 1 ||
                        marking.memory_mgmnt_control_operation == 3)
                        marking.difference_of_pic_nums_minus1 = ExpGolomb::get(br);

                    if (marking.memory_mgmnt_control_operation == 2)
                        marking.long_term_pic_num = ExpGolomb::get(br);

                    if (marking.memory_mgmnt_control_operation == 3 ||
                        marking.memory_mgmnt_control_operation == 6)
                        marking.long_term_frame_idx = ExpGolomb::get(br);

                    if (marking.memory_mgmnt_control_operation == 4)
                        marking.max_long_term_frame_idx_plus1 = ExpGolomb::get(br);

                    if (marking.memory_mgmnt_control_operation > 6)
                        return false;
                }
            }
        }
        return br->isOk();
    }
    bool SliceHeader::read(NalUnit * nalUnit, SPS * sps, PPS * pps) {
        auto profileIdcInfoExt = dynamic_cast<SPS::ProfileIdcInfoExt *>(sps->profileIdcInfo.get());
        if (profileIdcInfoExt && profileIdcInfoExt->separateColourPlaneFlag) {
            // "Interlaced streams not supported"
            return false;
        }
        BufferStream bs(nalUnit->getNalUnitPointer(), nalUnit->getNalUnitSize());
        BitStream br(&bs);
        first_mb_in_slice = ExpGolomb::get(&br);
        slice_type = ExpGolomb::get(&br);
        if (slice_type >= 10) {
            return false;
        }
        pic_parameter_set_id = ExpGolomb::get(&br);

        frame_num = br.get<uint16_t>(sps->log2MaxFrameNumMinus4 + 4);
        if (!sps->frameMbsOnlyFlag) {
            field_pic_flag = br.get();
            if (field_pic_flag) {
                // "Interlaced streams not supported"
                return false;
            }
        }
        auto idr = nalUnit->getH264Type() == NAL_TYPE_IDR;
        if (idr) {
            idr_pic_id = ExpGolomb::get(&br);
        }
        if (sps->picOrderCountType->type == 0) {
            auto picOrderCountType0 = dynamic_cast<SPS::PicOrderCountType0 *>(sps->picOrderCountType.get());
            pic_order_cnt_lsb = br.get<uint32_t>(
                    picOrderCountType0->log2MaxPicOrderCntLsbMinus4 + 4);
            if (pps->bottomFieldPicOrderInFramePresentFlag && !field_pic_flag) {
                delta_pic_order_cnt_bottom = ExpGolomb::getSigned(&br);
            }
        } else if (sps->picOrderCountType->type == 1) {
            auto picOrderCountType1 = dynamic_cast<SPS::PicOrderCountType1 *>(sps->picOrderCountType.get());
            if (!picOrderCountType1->deltaPicOrderAlwaysZeroFlag) {
                delta_pic_order_cnt0 = ExpGolomb::getSigned(&br);
                if (pps->bottomFieldPicOrderInFramePresentFlag && !field_pic_flag) {
                    delta_pic_order_cnt1 = ExpGolomb::getSigned(&br);
                }
            }
        }
        if (pps->redundant_pic_cnt_present_flag) {
            redundant_pic_cnt = ExpGolomb::get(&br);
            if (redundant_pic_cnt >= 128) {
                return false;
            }
        }

        if (isBSlice()) {
            direct_spatial_mv_pred_flag = br.get();
        }
        if (isPSlice() || isSPSlice() || isBSlice()) {
            num_ref_idx_active_override_flag = br.get();
            if (num_ref_idx_active_override_flag) {
                num_ref_idx_l0_active_minus1 = ExpGolomb::get(&br);
                if (isBSlice()) {
                    num_ref_idx_l1_active_minus1 = ExpGolomb::get(&br);
                }
            } else {
                num_ref_idx_l0_active_minus1 = pps->num_ref_idx_l0_default_active_minus1;
                if (isBSlice()) {
                    num_ref_idx_l1_active_minus1 = pps->num_ref_idx_l1_default_active_minus1;
                }
            }
        }
        auto max = field_pic_flag ? 32 : 16;
        if (num_ref_idx_l0_active_minus1 >= max ||
                num_ref_idx_l1_active_minus1 >= max) {
            return false;
        }
        if (nalUnit->getH264Type() == NAL_TYPE_SLICE_EXT) {
            return false;
        }
        if (!parseRefPicListModifications(&br)) {
            return false;
        }
        if ((pps->weighted_pred_flag && (isPSlice() || isSPSlice())) ||
            (pps->weighted_bipred_idc == 1 && isBSlice())) {
            if (!parsePredWeightTable(&br, sps)) {
                return false;
            }
        }
        if (nalUnit->getNalRefIdc() != 0) {
            if (!parseDecRefPicMarking(&br, idr)) {
                return false;
            }
        }

        return br.isOk();
    }

    int8_t SliceHeader::getLumaWeightDenom() const {
        return (int8_t)(1 << luma_log2_weight_denom);
    }

    int8_t SliceHeader::getChromaWeightDenom() const {
        return (int8_t)(1 << chroma_log2_weight_denom);
    }

} // h26x