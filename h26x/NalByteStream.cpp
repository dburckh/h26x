//
// Created by dburc on 12/8/2024.
//

#include "NalByteStream.h"
#include "BufferStream.h"

namespace h26x {
    NalByteStream::NalByteStream(ByteStream &byteStream): mByteStream(byteStream) {}

    size_t NalByteStream::skip(size_t s) {
        size_t skipped = 0;
        uint8_t current;
        while (s > 0 && read(&current) && mByteStream.skip(1)) {
            skipped++;
            mPrior2 = mPrior1;
            mPrior1 = current;
            if (mPrior2 == 0 && mPrior1 == 0) {
                if (read(&current)) {
                    if (current == 3) {
                        continue;
                    }
                } else {
                    break;
                }
            }
            s--;
        }
        return skipped;
    }

    size_t NalByteStream::available() const {
        return mByteStream.available();
    }

    size_t NalByteStream::position() const {
        return mByteStream.position();
    }

    bool NalByteStream::read(uint8_t *byte) const {
        return mByteStream.read(byte);
    }

    bool NalByteStream::write(uint8_t byte) {
        if (mPrior1 == 0 && mPrior2 == 0 && byte <= 3) {
            if (mByteStream.write(3)) {
                mByteStream.skip(1);
                mPrior1 = 3;
            } else {
                return false;
            }
        }
        return mByteStream.write(byte);
    }

    bool NalByteStream::isWrite() const {
        return mByteStream.isWrite();
    }
} // h26x