//
// Created by dburc on 12/13/2024.
//

#include "RWNalByteStream.h"

namespace h26x {
    RWNalByteStream::RWNalByteStream(RWByteStream *byteStream):
    NalByteStream(byteStream), mpRWByteStream(byteStream) {
    }

    bool RWNalByteStream::write(uint8_t byte) {
        if (mPrior1 == 0 && mPrior2 == 0 && byte <= 3) {
            if (mpRWByteStream->write(3)) {
                mpRWByteStream->skip(1);
                mPrior1 = 3;
            } else {
                return false;
            }
        }
        return mpRWByteStream->write(byte);
    }

    size_t RWNalByteStream::skip(size_t s) {
        return NalByteStream::skip(s);
    }

    size_t RWNalByteStream::available() const {
        return NalByteStream::available();
    }

    size_t RWNalByteStream::position() const {
        return NalByteStream::position();
    }

    bool RWNalByteStream::read(uint8_t *byte) const {
        return NalByteStream::read(byte);
    }
} // h26x