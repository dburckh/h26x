//
// Created by dburc on 1/20/2025.
//
#include <gtest/gtest.h>
#include <fstream>
#include <memory>
#include "ByteStream.h"
#include "SliceHeader.h"
#include "NalUnitFinder.h"
#include "NalByteStream.h"
#include "Common.h"
#include "BufferStream.h"
#include "PictureCounter.h"

using namespace h26x;

//------------------------------------------------------------------------------
TEST(pictureCounterTest, fromStream) {
    std::ifstream myfile("bbb.h264", std::ios::binary | std::ios::ate);
    ASSERT_TRUE(myfile);
    std::streamsize size = myfile.tellg();
    myfile.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    EXPECT_TRUE(myfile.read(buffer.data(), size));

    SPS sps;
    PPS pps;

    NalUnitFinder finder((const uint8_t *)(buffer.data()), size);

    {
        auto nalUnit = finder.findNalUnit4();
        EXPECT_EQ(NAL_TYPE_SPS, nalUnit->getH264Type());
        BufferStream bufferStream(nalUnit->getNalUnitPtr(), nalUnit->getNalUnitSize());
        NalByteStream nalByteStream(bufferStream);
        BitStream bitStream(nalByteStream);
        EXPECT_TRUE(sps.read(bitStream));
    }
    {
        auto nalUnit = finder.findNalUnit4();
        EXPECT_EQ(NAL_TYPE_PPS, nalUnit->getH264Type());
        BufferStream bufferStream(nalUnit->getNalUnitPtr(), nalUnit->getNalUnitSize());
        NalByteStream nalByteStream(bufferStream);
        auto bitStream = BitStream(nalByteStream);
        EXPECT_TRUE(pps.read(bitStream));
    }
    {
        auto nalUnit = finder.findNalUnit4();
        EXPECT_EQ(NAL_TYPE_SEI, nalUnit->getH264Type());
    }
    PictureCounter pictureCounter;
    {
        auto nalUnit = finder.findNalUnit4();
        EXPECT_EQ(NAL_TYPE_IDR, nalUnit->getH264Type());

        SliceHeader sliceHeader;
        EXPECT_TRUE(sliceHeader.read(*nalUnit, sps, pps));
        EXPECT_EQ(0, pictureCounter.getPictureCount(*nalUnit, sps, pps));
    }
    {
        auto nalUnit = finder.findNalUnit4();
        EXPECT_EQ(NAL_TYPE_NON_IDR, nalUnit->getH264Type());

        SliceHeader sliceHeader;
        EXPECT_TRUE(sliceHeader.read(*nalUnit, sps, pps));
        EXPECT_EQ(12, pictureCounter.getPictureCount(*nalUnit, sps, pps));
    }
    {
        auto nalUnit = finder.findNalUnit4();
        EXPECT_EQ(NAL_TYPE_NON_IDR, nalUnit->getH264Type());

        SliceHeader sliceHeader;
        EXPECT_TRUE(sliceHeader.read(*nalUnit, sps, pps));
        EXPECT_EQ(6, pictureCounter.getPictureCount(*nalUnit, sps, pps));
    }
}