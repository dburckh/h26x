//
// Created by dburc on 11/30/2024.
//

#include "BitStream.h"
#include <algorithm>

namespace h26x {
    BitStream::BitStream(ByteStream *byteStream):mByteStream(byteStream) {
        mBitsRemaining = setWork() ? 8 : 0;
    }

    uint8_t BitStream::_get(uint8_t bits) {
        switch (bits) {
            case 8:
                return mWork;
            case 0:
                mOk = false;
                return 0;
            default:
                return mWork & ALL_MASKS[bits];
        }
    }

    bool BitStream::get() {
        if (mBitsRemaining == 0) {
            return mOk = false;
        }
        bool bit = _get(mBitsRemaining) >> (mBitsRemaining - 1);
        if (mBitsRemaining == 1) {
            advanceBuffer();
        } else {
            mBitsRemaining--;
        }
        return bit;
    }

    template<typename T>
    T BitStream::get(uint8_t bits) {
        if (!available(bits)) {
            mOk = false;
            return 0;
        }

        T v = 0;
        while (bits > 0) {
            T work = _get(mBitsRemaining);
            if (bits >= mBitsRemaining) {
                // We require more bits than are available
                bits -= mBitsRemaining;
                v = (v << mBitsRemaining) | work;
                advanceBuffer();
            } else {
                // We require few bits than we have
                v <<= bits;
                mBitsRemaining -= bits;
                v |= (work >> mBitsRemaining);
                if (mBitsRemaining == 0) {
                    advanceBuffer();
                }
                break;
            }
        }
        return v;
    }

    bool BitStream::setWork() {
        return mByteStream->read(&mWork);
    }

    void BitStream::advanceBuffer() {
        flush();
        mBitsRemaining = mByteStream->skip(1) && setWork() ? 8 : 0;
    }

    size_t BitStream::available() const {
        if (mBitsRemaining) {
            return (mByteStream->available() - 1) * 8 + mBitsRemaining;
        } else {
            return mByteStream->available() * 8;
        }
    }

    size_t BitStream::skip(size_t s) {
        if (s < mBitsRemaining) {
            mBitsRemaining -= s;
            return s;
        } else if (s == mBitsRemaining) {
            advanceBuffer();
            return s;
        } else {
            flush();
        }

        auto toSkip = s;
        toSkip -= mBitsRemaining;
        mBitsRemaining = 0;
        toSkip -= mByteStream->skip(toSkip / 8) * 8;
        // This should only be >= 8 if we ran into the end of stream
        if (toSkip < 8) {
            advanceBuffer();
            if (toSkip < mBitsRemaining) {
                mBitsRemaining -= toSkip;
                return s;
            }
        }
        return s - toSkip;
    }

    bool BitStream::available(uint8_t bits) const {
        if (mBitsRemaining >= bits) {
            return true;
        }
        return available() >= bits;
    }

    bool BitStream::isOk() const {
        return mOk;
    }

    size_t BitStream::position() const {
        if (mBitsRemaining) {
            return (mByteStream->position() + 1) * 8 - mBitsRemaining;
        } else {
            return mByteStream->position() * 8;
        }
    }

    void BitStream::set(bool v) {
        if (mBitsRemaining == 0) {
            mOk = false;
            return;
        }
        if (v) {
            mWork |= 1 << (mBitsRemaining - 1);
        } else {
            mWork &= ~(1 << (mBitsRemaining - 1));
        }
        mWrite = true;
        if (mBitsRemaining == 1) {
            advanceBuffer();
        } else {
            mBitsRemaining--;
        }
    }

    template<typename T>
    void BitStream::set(T v, uint8_t bits) {
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

    void BitStream::setTrailingBits(bool v) {
        if (mBitsRemaining != 0 && mBitsRemaining != 8) {
            uint8_t fill = v ? 0x7f : 0;
            set(fill, mBitsRemaining);
        }
    }

    void BitStream::flush() {
        if (mWrite) {
            if (mByteStream->write(mWork)) {
                mWrite = false;
            }
        }
    }
    int BitStream::SINGLE_MASKS[9] = {0, 1, 2, 4, 8, 16, 32, 64, 128};
    int BitStream::ALL_MASKS[9] = {0, 1, 3, 7, 15, 31, 63, 127, 255};

    template uint8_t BitStream::get(uint8_t);
    template uint16_t BitStream::get(uint8_t);
    template uint32_t BitStream::get(uint8_t);
    template uint64_t BitStream::get(uint8_t);

    template void BitStream::set(int, uint8_t);

    template void BitStream::set(uint8_t, uint8_t);
    template void BitStream::set(uint16_t, uint8_t);
    template void BitStream::set(uint32_t, uint8_t);
    template void BitStream::set(uint64_t, uint8_t);
} // h26x