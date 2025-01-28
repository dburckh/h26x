//
// Created by dburc on 11/24/2024.
//
#include <gtest/gtest.h>
#include "../include/NalUnitFinder.h"

using namespace h26x;
//------------------------------------------------------------------------------
TEST(nalUnitTest, find) {

    const uint8_t data[] = {
            0x00, 0x00, 0x00, 0x01, 0x67, 0x64, 0x00, 0x1F, 0xAC, 0x72, 0x84, 0x40,
            0xA0, 0x2F, 0xF9, 0x70, 0x11, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00,
            0x03, 0x00, 0x3C, 0x0F, 0x18, 0x31, 0x84, 0x60, 0x00, 0x00, 0x00, 0x01,
            0x68, 0xE8, 0x43, 0x94, 0xB2, 0x2C, 0x00, 0x00, 0x00, 0x01, 0x06, 0x05
    };

    NalUnitFinder nuf(data, sizeof(data));

    EXPECT_TRUE(nuf.findPrefix4());
    EXPECT_EQ(data, nuf.getPrefixPtr());
    EXPECT_EQ(4, nuf.getPrefixSize());
    EXPECT_EQ(NAL_TYPE_SPS, NalUnit::getH264Type(nuf.getNalUnitType()));

    // auto nalUnit = nuf.findNalUnit4();
    // EXPECT_EQ(NAL_TYPE_SPS, nalUnit->getH264Type());

    NalUnitFinder lastNuf = nuf;

    EXPECT_TRUE(nuf.findPrefix4());
    EXPECT_EQ(data + 32, nuf.getPrefixPtr());
    EXPECT_EQ(NAL_TYPE_PPS, NalUnit::getH264Type(nuf.getNalUnitType()));

    EXPECT_EQ(32, nuf.getPrefixPtr() - lastNuf.getPrefixPtr());

    EXPECT_TRUE(nuf.findPrefix4());
    EXPECT_EQ(data + 42, nuf.getPrefixPtr());

    EXPECT_FALSE(nuf.findPrefix4());
}

