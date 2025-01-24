//
// Created by dburc on 11/16/2024.
//
#include <gtest/gtest.h>
#include "BitStream.h"
#include "SPS.h"
#include "BufferStream.h"
#include "Common.h"
#include "NalByteStream.h"

using namespace h26x;

//------------------------------------------------------------------------------
TEST(spsTest, readSps)
{
    const uint8_t data[] = {0x42, 0x00, 0x1E, 0x9A, 0x74, 0x04, 0x01,
                            0x8D, 0x08, 0x00, 0x00, 0x1F, 0x40, 0x00,
                            0x06, 0x1A, 0x84, 0x78, 0xB1, 0x75};

    BufferStream bufferStream(data, sizeof(data));
    BitStream br(bufferStream);
    SPS sps;
    sps.read(br);

    EXPECT_EQ(66, sps.profileIdc);

    EXPECT_FALSE(sps.constraintSet0Flag);
    EXPECT_FALSE(sps.constraintSet1Flag);
    EXPECT_FALSE(sps.constraintSet2Flag);
    EXPECT_FALSE(sps.constraintSet3Flag);
    EXPECT_FALSE(sps.constraintSet4Flag);
    EXPECT_FALSE(sps.constraintSet5Flag);
    EXPECT_EQ(30, sps.levelIdc);
    EXPECT_EQ(0, sps.seqParameterSetId);
    EXPECT_EQ(5, sps.log2MaxFrameNumMinus4);
    auto picOrderCountType = dynamic_cast<SPS::PicOrderCountType0*>(sps.picOrderCountType.get());
    EXPECT_TRUE(picOrderCountType);
    EXPECT_EQ(0, picOrderCountType->type);
    EXPECT_EQ(6, picOrderCountType->log2MaxPicOrderCntLsbMinus4);
    EXPECT_EQ(1, sps.numRefFrames);
    EXPECT_FALSE(sps.gapsInFrameNumValueAllowedFlag);
    EXPECT_EQ(31, sps.picWidthInMbsMinus1);
    EXPECT_EQ(23, sps.picHeightInMapUnitsMinus1);
    EXPECT_TRUE(sps.frameMbsOnlyFlag);
    EXPECT_TRUE(sps.direct8x8InferenceFlag);
    EXPECT_FALSE(sps.frameCropOffset);

    EXPECT_TRUE(sps.vuiParameters);
    EXPECT_FALSE(sps.vuiParameters->aspectRatio);
    EXPECT_FALSE(sps.vuiParameters->overscanInfoPresentFlag);
    EXPECT_FALSE(sps.vuiParameters->videoSignalType);
    EXPECT_FALSE(sps.vuiParameters->chromaLoc);
    EXPECT_TRUE(sps.vuiParameters->timing);
    EXPECT_EQ(1000, sps.vuiParameters->timing->numUnitsInTick);
    EXPECT_EQ(50000, sps.vuiParameters->timing->timeScale);
    EXPECT_TRUE(sps.vuiParameters->timing->fixedFrameRateFlag);
    EXPECT_FALSE(sps.vuiParameters->nalHrdParameters);
    EXPECT_FALSE(sps.vuiParameters->vclHrdParameters);
    EXPECT_FALSE(sps.vuiParameters->picStructPresentFlag);
    EXPECT_TRUE(sps.vuiParameters->bitstreamRestriction);
    EXPECT_TRUE(sps.vuiParameters->bitstreamRestriction->motionVectorsOverPicBoundariesFlag);
    EXPECT_EQ(0, sps.vuiParameters->bitstreamRestriction->maxBytesPerPicDenom);
    EXPECT_EQ(0, sps.vuiParameters->bitstreamRestriction->maxBitsPerMbDenom);
    EXPECT_EQ(10, sps.vuiParameters->bitstreamRestriction->log2maxMvLengthHorizontal);
    EXPECT_EQ(10, sps.vuiParameters->bitstreamRestriction->log2maxMvLengthVertical);
    EXPECT_EQ(0, sps.vuiParameters->bitstreamRestriction->numReorderFrames);
    EXPECT_EQ(1, sps.vuiParameters->bitstreamRestriction->maxDecFrameBuffering);

    std::vector<uint8_t> buffer(sizeof(data));
    BufferStream rwbs(buffer.data(), buffer.capacity());
    BitStream bw(rwbs);

    sps.write(bw);
    EXPECT_EQ(br.position(), bw.position());

    bw.flush();
    EXPECT_TRUE(bw.isOk());

    for (size_t i = 0; i < sizeof(data); ++i) {
        EXPECT_EQ(data[i], buffer[i]) << "Data differ at index " << i;
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TEST(spsTest, readCommonSps)
{

    const uint8_t data[] = SPS_DATA


    NalByteStream nbs(*(new BufferStream(data, sizeof(data))));
    BitStream br(nbs);
    SPS sps;
    sps.read(br);

    EXPECT_EQ(100, sps.profileIdc);

    EXPECT_FALSE(sps.constraintSet0Flag);
    EXPECT_FALSE(sps.constraintSet1Flag);
    EXPECT_FALSE(sps.constraintSet2Flag);
    EXPECT_FALSE(sps.constraintSet3Flag);
    EXPECT_FALSE(sps.constraintSet4Flag);
    EXPECT_FALSE(sps.constraintSet5Flag);
    EXPECT_EQ(31, sps.levelIdc);
    EXPECT_EQ(0, sps.seqParameterSetId);
    EXPECT_EQ(1, sps.profileIdcInfo->chromaFormatIdc);
    EXPECT_EQ(2, sps.log2MaxFrameNumMinus4);
    auto picOrderCountType = dynamic_cast<SPS::PicOrderCountType0*>(sps.picOrderCountType.get());
    EXPECT_TRUE(picOrderCountType);
    EXPECT_EQ(0, picOrderCountType->type);
    EXPECT_EQ(4, picOrderCountType->log2MaxPicOrderCntLsbMinus4);
    EXPECT_EQ(16, sps.numRefFrames);
    EXPECT_FALSE(sps.gapsInFrameNumValueAllowedFlag);
    EXPECT_EQ(39, sps.picWidthInMbsMinus1);
    EXPECT_EQ(22, sps.picHeightInMapUnitsMinus1);
    EXPECT_TRUE(sps.frameMbsOnlyFlag);
    EXPECT_TRUE(sps.direct8x8InferenceFlag);
    EXPECT_TRUE(sps.frameCropOffset);
    auto frameCropOffset = sps.frameCropOffset.get();
    EXPECT_EQ(0, frameCropOffset->left);
    EXPECT_EQ(0, frameCropOffset->right);
    EXPECT_EQ(0, frameCropOffset->top);
    EXPECT_EQ(4, frameCropOffset->bottom);

    EXPECT_TRUE(sps.vuiParameters);
    auto aspectRatio = sps.vuiParameters->aspectRatio.get();
    EXPECT_EQ(1, aspectRatio->aspectRationIdc);
    EXPECT_FALSE(sps.vuiParameters->overscanInfoPresentFlag);
    EXPECT_FALSE(sps.vuiParameters->videoSignalType);
    EXPECT_FALSE(sps.vuiParameters->chromaLoc);
    EXPECT_TRUE(sps.vuiParameters->timing);
    EXPECT_EQ(1, sps.vuiParameters->timing->numUnitsInTick);
    EXPECT_EQ(60, sps.vuiParameters->timing->timeScale);
    EXPECT_FALSE(sps.vuiParameters->timing->fixedFrameRateFlag);
    EXPECT_FALSE(sps.vuiParameters->nalHrdParameters);
    EXPECT_FALSE(sps.vuiParameters->vclHrdParameters);
    EXPECT_FALSE(sps.vuiParameters->picStructPresentFlag);
    if (sps.vuiParameters->bitstreamRestriction) {
        EXPECT_TRUE(sps.vuiParameters->bitstreamRestriction->motionVectorsOverPicBoundariesFlag);
        EXPECT_EQ(0, sps.vuiParameters->bitstreamRestriction->maxBytesPerPicDenom);
        EXPECT_EQ(0, sps.vuiParameters->bitstreamRestriction->maxBitsPerMbDenom);
        EXPECT_EQ(11, sps.vuiParameters->bitstreamRestriction->log2maxMvLengthHorizontal);
        EXPECT_EQ(11, sps.vuiParameters->bitstreamRestriction->log2maxMvLengthVertical);
        EXPECT_EQ(2, sps.vuiParameters->bitstreamRestriction->numReorderFrames);
        EXPECT_EQ(16, sps.vuiParameters->bitstreamRestriction->maxDecFrameBuffering);
    }
    else {
        FAIL();
    }

    std::vector<uint8_t> buffer(sizeof(data));
    BufferStream bufferStream(buffer.data(), buffer.capacity());
    NalByteStream rwnbs(bufferStream);
    BitStream bw(rwnbs);

    sps.write(bw);
    // Writer writes to end of last byte, so we need to round up to nearest byte
    auto byteEnd = (br.position() + 7) & 0xfff8;
    EXPECT_EQ(byteEnd, bw.position());

    EXPECT_TRUE(bw.isOk());

    for (size_t i = 0; i < sizeof(data); ++i) {
        EXPECT_EQ(data[i], buffer[i]) << "Data differ at index " << i;
    }
}