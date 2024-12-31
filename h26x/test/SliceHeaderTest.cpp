//
// Created by dburc on 11/30/2024.
//
#include <gtest/gtest.h>
#include "../include/ByteStream.h"
#include "../include/SliceHeader.h"
#include "Common.h"

using namespace h26x;

//------------------------------------------------------------------------------
TEST(sliceHeaderTest, readNonIdr) {
    const uint8_t ppsData[] = PPS_DATA

    ByteStream ppsRobs(ppsData, sizeof(ppsData));
    BitStream ppsBr(&ppsRobs);
    PPS pps;
    pps.read(&ppsBr);

    const uint8_t spsData[] = SPS_DATA

    ByteStream spsRobs(spsData, sizeof(spsData));
    BitStream spsBr(&spsRobs);
    SPS sps;
    sps.read(&spsBr);

    const uint8_t sliceData[48] = SLICE_DATA

    NalUnit sliceNalUnit(sliceData, sizeof(sliceData), 4);
    SliceHeader sliceHeader;
    sliceHeader.read(&sliceNalUnit, &sps, &pps);
}