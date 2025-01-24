//
// Created by dburc on 11/26/2024.
//
#include <gtest/gtest.h>
#include "Common.h"
#include "../include/BitStream.h"
#include "../include/BufferStream.h"
#include "../include/PPS.h"

using namespace h26x;

//------------------------------------------------------------------------------
TEST(ppsTest, readPps) {
    const uint8_t data[] = PPS_DATA

    BufferStream bs(data, sizeof(data));
    BitStream br(bs);
    PPS pps;
    pps.read(br);

    EXPECT_EQ(0, pps.picParameterSetId);
    EXPECT_EQ(0, pps.seqParameterSetId);
    EXPECT_TRUE(pps.entropyCodingModeFlag);
    EXPECT_FALSE(pps.bottomFieldPicOrderInFramePresentFlag);
    EXPECT_EQ(0, pps.num_slice_groups_minus1);
    EXPECT_EQ(15, pps.num_ref_idx_l0_default_active_minus1);
    EXPECT_EQ(0, pps.num_ref_idx_l1_default_active_minus1);
    EXPECT_EQ(2, pps.weighted_bipred_idc);
    EXPECT_EQ(1, pps.pic_init_qp_minus26);
    EXPECT_EQ(0, pps.pic_init_qs_minus26);
    EXPECT_EQ(-2, pps.chroma_qp_index_offset);
    EXPECT_TRUE(pps.deblocking_filter_control_present_flag);
    EXPECT_FALSE(pps.constrained_intra_pred_flag);
    EXPECT_FALSE(pps.redundant_pic_cnt_present_flag);
}