//
// Created by dburc on 11/30/2024.
//

#ifndef H26X_UTILS_BITSTREAM_H
#define H26X_UTILS_BITSTREAM_H
#include "Stream.h"
#include "ByteStream.h"

namespace h26x {

    class BitStream:public Stream {
    public:
        explicit BitStream(ByteStream * byteStream);
        bool get();
        template<typename T>
        T get(uint8_t bits);
        uint8_t _get(uint8_t bits);

        [[nodiscard]] bool isOk() const;
        [[nodiscard]] size_t available() const override;
        [[nodiscard]] size_t position() const override;
        size_t skip(size_t s) override;

    protected:
        virtual void advanceBuffer();
        [[nodiscard]] bool available(uint8_t bits) const;

        bool mOk {true};
        uint8_t mWork {0};
        uint8_t mBitsRemaining;

    private:
        bool setWork();
        ByteStream * mByteStream;
    };

} // h26x

#endif //H26X_UTILS_BITSTREAM_H
