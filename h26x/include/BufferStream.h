//
// Created by dburc on 12/8/2024.
//

#ifndef H26X_UTILS_BUFFERSTREAM_H
#define H26X_UTILS_BUFFERSTREAM_H
#include "ByteStream.h"

namespace h26x {

    class BufferStream:public ByteStream {
    public:
        BufferStream(const uint8_t * pBuffer, size_t size);
        BufferStream(uint8_t * pBuffer, size_t size);
        [[nodiscard]] size_t available() const override;
        [[nodiscard]] size_t position() const override;
        size_t skip(size_t s) override;
        bool read(uint8_t *byte) const override;
        bool write(uint8_t bytes) override;

    private:
        const size_t mSize;
        size_t mPosition {0};
        const uint8_t *mpBuffer;
        uint8_t * mpRWBuffer;

    };

} // h26x

#endif //H26X_UTILS_BUFFERSTREAM_H
