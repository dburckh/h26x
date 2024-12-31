//
// Created by dburc on 12/8/2024.
//

#ifndef H26X_UTILS_RWBYTESTREAM_H
#define H26X_UTILS_RWBYTESTREAM_H
#include "ByteStream.h"
#include <cstdint>

namespace h26x {
    class RWByteStream: public ByteStream {
    public:
        /**
         * Write the current byte.  Does not advance position()
         * @param byte by value
         * @return true if a byte was written (not at end of buffer)
         */
        virtual bool write(uint8_t byte) = 0;
    };
}


#endif //H26X_UTILS_RWBYTESTREAM_H
