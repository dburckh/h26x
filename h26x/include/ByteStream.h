//
// Created by dburc on 11/30/2024.
//

#ifndef H26X_UTILS_BYTESTREAM_H
#define H26X_UTILS_BYTESTREAM_H

#include "Stream.h"

namespace h26x {

    class ByteStream: public Stream {
    public:
        virtual bool read(uint8_t *byte) const = 0;
        /**
         * Write the current byte.  Does not advance position()
         * @param byte by value
         * @return true if a byte was written (not at end of buffer)
         */
        virtual bool write(uint8_t byte) = 0;
    };

} // h26x

#endif //H26X_UTILS_BYTESTREAM_H
