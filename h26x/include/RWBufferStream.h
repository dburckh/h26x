//
// Created by dburc on 12/7/2024.
//

#ifndef H26X_UTILS_RWBUFFERSTREAM_H
#define H26X_UTILS_RWBUFFERSTREAM_H

#include "RWByteStream.h"
#include "BufferStream.h"

namespace h26x {
    class RWBufferStream: public BufferStream, public RWByteStream {
    public:
        RWBufferStream(uint8_t *pBuffer, size_t size);
        bool write(uint8_t bytes) override;

        // It won't compile unless I call the explicitly ancestor from the descendant
        // Any C++ guru's have a better solution??
        [[nodiscard]] size_t position() const override;
        [[nodiscard]] size_t available() const override;
        size_t skip(size_t s) override;
        bool read(uint8_t *byte) const override;
    private:
        uint8_t * mpRWBuffer;
    };
}

#endif //H26X_UTILS_RWBUFFERSTREAM_H
