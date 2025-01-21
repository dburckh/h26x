//
// Created by dburc on 12/8/2024.
//

#include "ExpGolomb.h"
#include <algorithm>

namespace h26x {

uint32_t ExpGolomb::get(BitStream &bitStream) {
        uint8_t leadingZeros = 0;
        while (bitStream.isOk() && !bitStream.get()) {
            leadingZeros++;
        }
        return (1 << leadingZeros) - 1 + (leadingZeros > 0 ? bitStream.get<uint32_t>(leadingZeros) : 0);
    }

    int32_t ExpGolomb::getSigned(BitStream &bitStream) {
        auto codeNum = get(bitStream);
        return ((codeNum % 2) == 0 ? -1 : 1) * (((int32_t) codeNum + 1) / 2);
    }

    void ExpGolomb::set(uint32_t v, BitStream &bitStream) {
        uint8_t bits = 0;
        int cumul = 0;
        for (uint8_t i = 0; i < 15; i++) {
            if (v < cumul + (1 << i)) {
                bits = i;
                break;
            }
            cumul += (1 << i);
        }
        bitStream.set(0, bits);
        bitStream.set(true);
        bitStream.set(v - cumul, bits);
    }

    void ExpGolomb::setSigned(int32_t v, BitStream &bitStream) {
        uint32_t uintV = v == 0 ? 0 : (abs(v) << 1) - (~v >> 31);
        set(uintV, bitStream);
    }
} // h26x