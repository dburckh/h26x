//
// Created by dburc on 12/8/2024.
//

#include "NalByteStream.h"

namespace h26x {
    NalByteStream::NalByteStream(ByteStream * byteStream):mByteStream(byteStream) {
    }

    size_t NalByteStream::skip(size_t s) {
        size_t skipped = 0;
        uint8_t current;
        while (s > 0) {
            if (mByteStream->read(&current) && mByteStream->skip(1)) {
                mPrior2 = mPrior1;
                mPrior1 = current;
                skipped++;
                s--;
            } else {
                break;
            }
        }
        return skipped;
    }

    size_t NalByteStream::available() const {
        return mByteStream->available();
    }

    size_t NalByteStream::position() const {
        return mByteStream->position();
    }

    bool NalByteStream::read(uint8_t *byte) const {
        if (mByteStream->read(byte)) {
            if (*byte == 3 && mPrior1 == 0 && mPrior2 == 0) {
                return mByteStream->skip(1) > 0 && read(byte);
            }
            return true;
        }
        return false;
    }

} // h26x