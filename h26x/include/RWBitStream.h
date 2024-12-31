//
// Created by dburc on 12/7/2024.
//

#ifndef H26X_UTILS_RWBITSTREAM_H
#define H26X_UTILS_RWBITSTREAM_H

#include "RWBufferStream.h"
#include "BitStream.h"

namespace h26x {

    class RWBitStream: public BitStream {
    public:
        explicit RWBitStream(RWByteStream * rwByteStream);
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
        size_t skip(size_t s) override;
    protected:
        void advanceBuffer() override;
    private:
        RWByteStream * mRWByteStream;
        bool mWrite {false};

        static int SINGLE_MASKS[9];
        static int ALL_MASKS[9];
    };

} // h26x

#endif //H26X_UTILS_RWBITSTREAM_H
