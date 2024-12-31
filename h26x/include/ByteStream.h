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
    };

} // h26x

#endif //H26X_UTILS_BYTESTREAM_H
