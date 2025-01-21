//
// Created by dburc on 11/25/2024.
//

#include "NalUnit.h"

namespace h26x {
    NalUnit::NalUnit(const uint8_t *pBuffer, size_t size, uint8_t nalTypeOffset):mpBuffer(pBuffer),mSize(size), mNalTypeOffset(nalTypeOffset) {

    }
    uint8_t h26x::NalUnit::getH264Type(uint8_t nalHeader) {
        return nalHeader & 0x1f;
    }

    uint8_t h26x::NalUnit::getH265Type(uint8_t nalHeader) {
        return nalHeader >> 1;
    }

    uint8_t NalUnit::getH264Type() const {
        return getH264Type(mpBuffer[mNalTypeOffset]);
    }

    const uint8_t *NalUnit::getNalTypePointer() const {
        return mpBuffer + mNalTypeOffset;
    }

    uint8_t NalUnit::getH265Type() const {
        return getH265Type(mpBuffer[mNalTypeOffset]);;
    }

    size_t NalUnit::getSize() const {
        return mSize;
    }

    const uint8_t *NalUnit::getNalUnitPointer() const {
        return mpBuffer + mNalTypeOffset + 1;
    }

    size_t NalUnit::getNalUnitSize() const {
        return mSize - mNalTypeOffset - 1;
    }

    uint8_t NalUnit::getNalRefIdc() const {
        // H264
        return (mpBuffer[mNalTypeOffset] & 0x60) >> 5;
    }

    BufferStream NalUnit::getPayload() const {
        return {getNalUnitPointer(), getNalUnitSize()};
    }

} // h26x