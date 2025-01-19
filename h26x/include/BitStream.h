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
        void set(bool v);
        template<typename T>
        void set(T v, uint8_t bits);
        /**
         * Set the trailing bits to true or false
         * @param v
         */
        void setTrailingBits(bool v);
        /**
         * Flush any remaining bits to the ByteSteam
         */
        void flush();

    private:
        void advanceBuffer();
        [[nodiscard]] bool available(uint8_t bits) const;

        bool setWork();
        ByteStream * mByteStream;

        bool mWrite {false};
        bool mOk {true};
        uint8_t mWork {0};
        uint8_t mBitsRemaining;

        static int SINGLE_MASKS[9];
        static int ALL_MASKS[9];
    };

} // h26x

#endif //H26X_UTILS_BITSTREAM_H
