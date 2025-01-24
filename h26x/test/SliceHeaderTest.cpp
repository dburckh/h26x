//
// Created by dburc on 11/30/2024.
//
#include <gtest/gtest.h>
#include "../include/ByteStream.h"
#include "../include/SliceHeader.h"
#include "Common.h"
#include "BufferStream.h"

using namespace h26x;

//------------------------------------------------------------------------------
TEST(sliceHeaderTest, readNonIdr) {
    const uint8_t ppsData[] = PPS_DATA

    BufferStream ppsBs(ppsData, sizeof(ppsData));
    BitStream ppsBr(ppsBs);
    PPS pps;
    pps.read(ppsBr);

    const uint8_t spsData[] = SPS_DATA
    BufferStream spsBs(spsData, sizeof(spsData));
    BitStream spsBr(spsBs);
    SPS sps;
    sps.read(spsBr);

    const uint8_t sliceData[48] = NIDR_SLICE_NAL

    NalUnit sliceNalUnit(sliceData, sizeof(sliceData), 4);
    SliceHeader sliceHeader;
    EXPECT_TRUE(sliceHeader.read(sliceNalUnit, sps, pps));

    EXPECT_EQ(5, sliceHeader.slice_type);
    EXPECT_EQ(5, sliceHeader.frame_num);
    EXPECT_EQ(40, sliceHeader.pic_order_cnt_lsb);
    EXPECT_TRUE(sliceHeader.num_ref_idx_active_override_flag);
    EXPECT_EQ(5, sliceHeader.num_ref_idx_l0_active_minus1);
    const auto chroma_weight_denom = sliceHeader.getChromaWeightDenom();
    for (int i=0;i<sliceHeader.num_ref_idx_l0_active_minus1+1;i++) {
        EXPECT_EQ(chroma_weight_denom, sliceHeader.pred_weight_table_l0->luma_weight[i]);
    }
    EXPECT_EQ(0, sliceHeader.pred_weight_table_l0->luma_offset[1]);
    EXPECT_EQ(-1, sliceHeader.pred_weight_table_l0->luma_offset[2]);

    EXPECT_EQ(0, sliceHeader.ref_list_l0_modifications[0].modification_of_pic_nums_idc);
    EXPECT_EQ(1, sliceHeader.ref_list_l0_modifications[0].abs_diff_pic_num_minus1);

    EXPECT_EQ(0, sliceHeader.ref_list_l0_modifications[1].modification_of_pic_nums_idc);
    EXPECT_EQ(2, sliceHeader.ref_list_l0_modifications[1].abs_diff_pic_num_minus1);

    EXPECT_EQ(1, sliceHeader.ref_list_l0_modifications[2].modification_of_pic_nums_idc);
    EXPECT_EQ(2, sliceHeader.ref_list_l0_modifications[2].abs_diff_pic_num_minus1);

    EXPECT_EQ(0, sliceHeader.ref_list_l0_modifications[3].modification_of_pic_nums_idc);
    EXPECT_EQ(0, sliceHeader.ref_list_l0_modifications[3].abs_diff_pic_num_minus1);

    EXPECT_EQ(0, sliceHeader.ref_list_l0_modifications[4].modification_of_pic_nums_idc);
    EXPECT_EQ(0, sliceHeader.ref_list_l0_modifications[4].abs_diff_pic_num_minus1);

    EXPECT_EQ(1, sliceHeader.ref_list_l0_modifications[5].modification_of_pic_nums_idc);
    EXPECT_EQ(2, sliceHeader.ref_list_l0_modifications[5].abs_diff_pic_num_minus1);
}
