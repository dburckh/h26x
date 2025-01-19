//
// Created by dburc on 12/8/2024.
//

#ifndef H26X_UTILS_NALBYTESTREAM_H
#define H26X_UTILS_NALBYTESTREAM_H
#include "BufferStream.h"

namespace h26x {

    class NalByteStream : public ByteStream {
    public:
        explicit NalByteStream(ByteStream * byteStream);
        size_t skip(size_t s) override;
        [[nodiscard]] size_t available() const override;
        [[nodiscard]] size_t position() const override;
        bool read(uint8_t *byte) const override;
        bool write(uint8_t byte) override;
    protected:
        // These need to be 1, not 0
        uint8_t mPrior1 {1};
        uint8_t mPrior2 {1};
    private:
        ByteStream * mByteStream;
    };

} // h26x

#endif //H26X_UTILS_NALBYTESTREAM_H
