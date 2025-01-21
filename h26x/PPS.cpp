//
// Created by dburc on 11/25/2024.
//

#include "include/PPS.h"
#include "ExpGolomb.h"

namespace h26x {
    bool PPS::read(BitStream &bs) {
        picParameterSetId = ExpGolomb::get(bs);
        seqParameterSetId = ExpGolomb::get(bs);
        entropyCodingModeFlag = bs.get();
        bottomFieldPicOrderInFramePresentFlag = bs.get();
        num_slice_groups_minus1 = ExpGolomb::get(bs);
        if (num_slice_groups_minus1 > 0) {
            auto sliceGroupMapType = ExpGolomb::get(bs);
            switch (sliceGroupMapType) {
                case 0:
                    for (int i=0;i<=num_slice_groups_minus1;i++) {
                        ExpGolomb::get(bs); //run_length_minus1
                    }
                    break;
                case 2:
                    for (int i=0;i<=num_slice_groups_minus1;i++) {
                        ExpGolomb::get(bs); //top_left
                        ExpGolomb::get(bs); //bottom_right
                    }
                    break;
                case 3:
                case 4:
                case 5:
                    bs.get(); //slice_group_change_direction_flag
                    ExpGolomb::get(bs); //slice_group_change_rate_minus1
                    break;
                case 6: {
                    auto pic_size_in_map_units_minus1 = ExpGolomb::get(bs);
                    for(int i = 0; i <= pic_size_in_map_units_minus1; i++ ) {
                        bs.get<uint32_t>(num_slice_groups_minus1 + 1);
                    }
                    break;
                }
            }
        }
        num_ref_idx_l0_default_active_minus1 = ExpGolomb::get(bs);
        num_ref_idx_l1_default_active_minus1 = ExpGolomb::get(bs);
        weighted_pred_flag = bs.get();
        weighted_bipred_idc = bs.get<uint8_t>(2);
        pic_init_qp_minus26 = ExpGolomb::getSigned(bs);
        pic_init_qs_minus26 = ExpGolomb::getSigned(bs);
        chroma_qp_index_offset = ExpGolomb::getSigned(bs);
        deblocking_filter_control_present_flag = bs.get();
        constrained_intra_pred_flag = bs.get();
        redundant_pic_cnt_present_flag = bs.get();
        return bs.isOk();
    }
} // h26x