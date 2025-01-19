//
// Created by dburc on 12/8/2024.
//

#ifndef H26X_UTILS_EXPGOLOMB_H
#define H26X_UTILS_EXPGOLOMB_H
#include "BitStream.h"

namespace h26x {

    class ExpGolomb {
    public:
        static uint32_t get(BitStream * bitStream);
        static int32_t getSigned(BitStream * bitStream);
        static void set(uint32_t v, BitStream * rwBitStream);
        static void setSigned(int32_t v, BitStream * rwBitStream);
    };

} // h26x

#endif //H26X_UTILS_EXPGOLOMB_H
