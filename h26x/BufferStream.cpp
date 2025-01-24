//
// Created by dburc on 12/8/2024.
//

#include "BufferStream.h"
#include <algorithm>

namespace h26x {
    BufferStream::BufferStream(const uint8_t *pBuffer, size_t size):
            mpBuffer(pBuffer),mWrite(false),mSize(size) {
    }
    BufferStream::BufferStream(uint8_t *pBuffer, size_t size):
            mpBuffer(pBuffer),mWrite(true),mSize(size) {
    }
    size_t BufferStream::position() const {
        return mPosition;
    }

    size_t BufferStream::skip(size_t s) {
        auto toSkip = std::min(s, available());
        mPosition += toSkip;
        return toSkip;
    }

    size_t BufferStream::available() const {
        return mSize - mPosition;
    }

    bool BufferStream::read(uint8_t *byte) const {
        if (mPosition >= mSize) {
            return false;
        }
        *byte = mpBuffer[mPosition];
        return true;
    }
    bool BufferStream::write(uint8_t byte) {
        if (mWrite && mPosition < mSize) {
            const_cast<uint8_t *>(mpBuffer)[mPosition] = byte;
            return true;
        }
        return false;
    }

    bool BufferStream::isWrite() const {
        return mWrite;
    }
} // h26x