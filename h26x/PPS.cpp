//
// Created by dburc on 11/25/2024.
//

#include "include/PPS.h"
#include "ExpGolomb.h"

namespace h26x {
    void PPS::read(BitStream * br) {
        picParameterSetId = ExpGolomb::get(br);
        seqParameterSetId = ExpGolomb::get(br);
        entropyCodingModeFlag = br->get();
        bottomFieldPicOrderInFramePresentFlag = br->get();
        num_slice_groups_minus1 = ExpGolomb::get(br);
        if (num_slice_groups_minus1 > 0) {
            auto sliceGroupMapType = ExpGolomb::get(br);
            switch (sliceGroupMapType) {
                case 0:
                    for (int i=0;i<=num_slice_groups_minus1;i++) {
                        ExpGolomb::get(br); //run_length_minus1
                    }
                    break;
                case 2:
                    for (int i=0;i<=num_slice_groups_minus1;i++) {
                        ExpGolomb::get(br); //top_left
                        ExpGolomb::get(br); //bottom_right
                    }
                    break;
                case 3:
                case 4:
                case 5:
                    br->get(); //slice_group_change_direction_flag
                    ExpGolomb::get(br); //slice_group_change_rate_minus1
                    break;
                case 6: {
                    auto pic_size_in_map_units_minus1 = ExpGolomb::get(br);
                    for(int i = 0; i <= pic_size_in_map_units_minus1; i++ ) {
                        br->get<uint32_t>(num_slice_groups_minus1 + 1);
                    }
                    break;
                }
            }
        }
        num_ref_idx_l0_default_active_minus1 = ExpGolomb::get(br);
        num_ref_idx_l1_default_active_minus1 = ExpGolomb::get(br);
        weighted_pred_flag = br->get();
        weighted_bipred_idc = br->get<uint8_t>(2);
        pic_init_qp_minus26 = ExpGolomb::getSigned(br);
        pic_init_qs_minus26 = ExpGolomb::getSigned(br);
        chroma_qp_index_offset = ExpGolomb::getSigned(br);
        deblocking_filter_control_present_flag = br->get();
        constrained_intra_pred_flag = br->get();
        redundant_pic_cnt_present_flag = br->get();
    }
} // h26x