//
// Created by dburc on 11/27/2024.
//

#ifndef H26X_UTILS_SLICEHEADER_H
#define H26X_UTILS_SLICEHEADER_H
#include <cstdint>
#include <vector>
#include "NalUnit.h"
#include "SPS.h"
#include "PPS.h"
#include "BitStream.h"

namespace h26x {
    struct H264ModificationOfPicNum {
        uint32_t modification_of_pic_nums_idc;
        union {
            uint32_t abs_diff_pic_num_minus1;
            uint32_t long_term_pic_num;
        };
    };
    struct H264WeightingFactors {
            int8_t *luma_weight;
            int8_t *luma_offset;
            int8_t (*chroma_weight)[2];
            int8_t (*chroma_offset)[2];
    };
    struct H264DecRefPicMarking {
            uint32_t memory_mgmnt_control_operation;
            uint32_t difference_of_pic_nums_minus1;
            uint32_t long_term_pic_num;
            uint32_t long_term_frame_idx;
            uint32_t max_long_term_frame_idx_plus1;
    };

    class SliceHeader {
    public:
        bool read(NalUnit * nalUnit, SPS * sps, PPS * pps);
        bool isPSlice() const;
        bool isBSlice() const;
        bool isSPSlice() const;
        bool isISlice() const;
        bool isSISlice() const;
        int8_t getLumaWeightDenom() const;
        int8_t getChromaWeightDenom() const;

        uint32_t first_mb_in_slice;
        uint32_t slice_type;
        uint32_t pic_parameter_set_id;
        int colour_plane_id;
        uint16_t frame_num;
        bool field_pic_flag = false;
        bool bottom_field_flag;
        uint32_t idr_pic_id {0};
        uint32_t pic_order_cnt_lsb {0};
        int32_t delta_pic_order_cnt_bottom {0};
        int32_t delta_pic_order_cnt0 {0};
        int32_t delta_pic_order_cnt1 {0};
        uint32_t redundant_pic_cnt {0};

        bool direct_spatial_mv_pred_flag = false;
        bool num_ref_idx_active_override_flag = false;
        uint32_t num_ref_idx_l0_active_minus1 {0};
        uint32_t num_ref_idx_l1_active_minus1 {0};

        bool ref_pic_list_modification_flag_l0 = false;
        bool ref_pic_list_modification_flag_l1 = false;

        std::vector<H264ModificationOfPicNum> ref_list_l0_modifications;
        std::vector<H264ModificationOfPicNum> ref_list_l1_modifications;

        uint32_t luma_log2_weight_denom {0};
        uint32_t chroma_log2_weight_denom {0};

        bool luma_weight_l0_flag = false;
        bool chroma_weight_l0_flag = false;
        std::unique_ptr<H264WeightingFactors> pred_weight_table_l0;

        bool luma_weight_l1_flag = false;
        bool chroma_weight_l1_flag = false;
        std::unique_ptr<H264WeightingFactors> pred_weight_table_l1;

        bool no_output_of_prior_pics_flag;
        bool long_term_reference_flag;

        bool adaptive_ref_pic_marking_mode_flag;
        std::vector<H264DecRefPicMarking> ref_pic_marking;

    private:
        static bool parseRefPicListModification(
                BitStream * br,
                uint32_t num_ref_idx_active_minus1,
                std::vector<H264ModificationOfPicNum> *ref_list_mods);
        bool parseWeightingFactors(
                BitStream * br,
                uint32_t num_ref_idx_active_minus1,
                uint8_t chroma_array_type,
                H264WeightingFactors* w_facts);
        bool parseRefPicListModifications(BitStream * br);
        bool parsePredWeightTable(BitStream * br, SPS * sps);

        bool parseDecRefPicMarking(BitStream *br, bool idr);
    };

} // h26x

#endif //H26X_UTILS_SLICEHEADER_H
