//
// Created by dburc on 11/30/2024.
//

#include "BitStream.h"

namespace h26x {
    BitStream::BitStream(ByteStream *byteStream):mByteStream(byteStream) {
        mBitsRemaining = setWork() ? 8 : 0;
    }

    uint8_t BitStream::_get(uint8_t bits) {
        switch (bits) {
            case 8:
                return mWork;
            case 7:
                return mWork & 0x7f;
            case 6:
                return mWork & 0x3f;
            case 5:
                return mWork & 0x1f;
            case 4:
                return mWork & 0xf;
            case 3:
                return mWork & 0x7;
            case 2:
                return mWork & 0x3;
            case 1:
                return mWork & 0x1;
            default:
                mOk = false;
                return 0;
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

    template uint8_t BitStream::get(uint8_t);
    template uint16_t BitStream::get(uint8_t);
    template uint32_t BitStream::get(uint8_t);
    template uint64_t BitStream::get(uint8_t);
} // h26x