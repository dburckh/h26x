//
// Created by dburc on 11/25/2024.
//

#ifndef H26X_UTILS_PPS_H
#define H26X_UTILS_PPS_H
#include "BitStream.h"

namespace h26x {

    /**
     * pic_parameter_set_rbsp( )
     */
    class PPS {
    public:

        uint32_t picParameterSetId {0};
        uint32_t seqParameterSetId {0};
        bool entropyCodingModeFlag = false;
        bool bottomFieldPicOrderInFramePresentFlag = false;
        uint32_t num_slice_groups_minus1 {0};
        uint32_t num_ref_idx_l0_default_active_minus1 {0};
        uint32_t num_ref_idx_l1_default_active_minus1 {0};
        bool weighted_pred_flag = false;
        uint8_t weighted_bipred_idc {0};
        int32_t pic_init_qp_minus26 {0};
        int32_t pic_init_qs_minus26 {0};
        int32_t chroma_qp_index_offset {0};
        bool deblocking_filter_control_present_flag = false;
        bool constrained_intra_pred_flag = false;
        bool redundant_pic_cnt_present_flag = false;
        void read(BitStream * bitStream);
    };

} // h26x

#endif //H26X_UTILS_PPS_H
