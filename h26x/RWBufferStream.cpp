//
// Created by dburc on 11/30/2024.
//

#include "RWBufferStream.h"

namespace h26x {
    RWBufferStream::RWBufferStream(uint8_t *pBuffer, size_t size): BufferStream(pBuffer, size),
        mpRWBuffer(pBuffer) {
    }

    bool RWBufferStream::write(uint8_t byte) {
        if (mPosition < mSize) {
            mpRWBuffer[mPosition] = byte;
            return true;
        }
        return false;
    }

    size_t RWBufferStream::position() const {
        return BufferStream::position();
    }

    size_t RWBufferStream::available() const {
        return BufferStream::available();
    }

    size_t RWBufferStream::skip(size_t s) {
        return BufferStream::skip(s);
    }

    bool RWBufferStream::read(uint8_t *byte) const {
        return BufferStream::read(byte);
    }

} // h26x