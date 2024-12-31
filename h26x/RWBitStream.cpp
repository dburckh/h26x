//
// Created by dburc on 12/7/2024.
//

#include "RWBitStream.h"
#include <algorithm>

namespace h26x {
    RWBitStream::RWBitStream(RWByteStream *rwByteStream):
    BitStream(rwByteStream), mRWByteStream(rwByteStream) {

    }

    void RWBitStream::set(bool v) {
        if (mBitsRemaining == 0) {
            mOk = false;
            return;
        }
        if (v) {
            mWork |= SINGLE_MASKS[mBitsRemaining];
        } else {
            mWork &= ~SINGLE_MASKS[mBitsRemaining];
        }
        mWrite = true;
        if (mBitsRemaining == 1) {
            advanceBuffer();
        } else {
            mBitsRemaining--;
        }
    }

    template<typename T>
    void RWBitStream::set(T v, uint8_t bits) {
        if (!available(bits)) {
            mOk = false;
            return;
        }
        while (bits > 0) {
            uint8_t bitsToSet = std::min(bits, mBitsRemaining);
            auto maskShift = mBitsRemaining - bitsToSet;
            auto mask = ALL_MASKS[bitsToSet] << maskShift;
            auto vShift = (bits - bitsToSet - maskShift);
            T vShifted;
            if (vShift > 0) {
                vShifted = v >> vShift;
            } else if (vShift == 0) {
                vShifted = v;
            } else {
                vShifted = v << -vShift;
            }
            //std::cout << "bitsToSet: " << (uint32_t)bitsToSet << " mask: " << mask << " maskShift: " << maskShift << " bits: " << (uint32_t)bits << " vShift:" << vShift << " vShifted: " << vShifted << '\n';
            mWork = (mWork & ~mask) | (vShifted & mask);
            mWrite = true;
            if (mBitsRemaining == bitsToSet) {
                advanceBuffer();
            } else {
                mBitsRemaining -= bitsToSet;
            }
            bits -= bitsToSet;
        }
    }

    void RWBitStream::setTrailingBits(bool v) {
        if (mBitsRemaining != 0 && mBitsRemaining != 8) {
            uint8_t fill = v ? 0x7f : 0;
            set(fill, mBitsRemaining);
        }
    }

    void RWBitStream::flush() {
        if (mWrite) {
            if (mRWByteStream->write(mWork)) {
                mWrite = false;
            }
        }
    }

    void RWBitStream::advanceBuffer() {
        flush();
        BitStream::advanceBuffer();
    }

    size_t RWBitStream::skip(size_t s) {
        if (s >= mBitsRemaining) {
            flush();
        }
        return BitStream::skip(s);
    }


    int RWBitStream::SINGLE_MASKS[9] = {0, 1, 2, 4, 8, 16, 32, 64, 128};
    int RWBitStream::ALL_MASKS[9] = {0, 1, 3, 7, 15, 31, 63, 127, 255};

    template void RWBitStream::set(int, uint8_t);

    template void RWBitStream::set(uint8_t, uint8_t);
    template void RWBitStream::set(uint16_t, uint8_t);
    template void RWBitStream::set(uint32_t, uint8_t);
    template void RWBitStream::set(uint64_t, uint8_t);
} // h26x