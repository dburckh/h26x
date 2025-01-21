//
// Created by dburc on 11/25/2024.
//

#ifndef H26X_UTILS_NALUNIT_H
#define H26X_UTILS_NALUNIT_H
//--------------------------------- H264 NAL Unit Types -----------------------
#define NAL_TYPE_NON_IDR            1
#define NAL_TYPE_SLICE_DATA_A       2
#define NAL_TYPE_SLICE_DATA_B       3
#define NAL_TYPE_SLICE_DATA_C       4
#define NAL_TYPE_IDR                5
// Supplemental enhancement information (Text Notes)
#define NAL_TYPE_SEI                6
#define NAL_TYPE_SPS                7
#define NAL_TYPE_PPS                8
// Access Unit Delimiter (Optional frame delimiter)
#define NAL_TYPE_AUD                9
#define NAL_TYPE_SLICE_EXT          20

//--------------------------------- H265 NAL Unit types -----------------------
#define IDR_W_RADL                  19
#define IDR_N_LP                    20
#define H265_NAL_VPS				32 // Video parameter set
#define H265_NAL_SPS				33 // Sequence parameter set
#define H265_NAL_PPS				34 // Picture parameter set
#include <cstdint>
#include "BufferStream.h"

namespace h26x {

    class NalUnit {
    public:
        NalUnit(const uint8_t * pBuffer, size_t size, uint8_t nalTypeOffset);
        static uint8_t getH264Type(uint8_t nalHeader);
        [[nodiscard]] uint8_t getH264Type() const;

        static uint8_t getH265Type(uint8_t nalHeader);
        [[nodiscard]] uint8_t getH265Type() const;

        [[nodiscard]] size_t getSize() const;
        [[nodiscard]] const uint8_t * getNalTypePointer() const;
        [[nodiscard]] BufferStream getPayload() const;
        /**
         * Return the pointer to start of the NAL Unit payload (after type)
         */
        [[nodiscard]] const uint8_t * getNalUnitPointer() const;
        /**
         * Return size the NAL Unit payload (after type)
         */
        [[nodiscard]] size_t getNalUnitSize() const;
        [[nodiscard]] uint8_t getNalRefIdc() const;

    private:
        const uint8_t * const mpBuffer;
        const size_t mSize;
        const uint8_t mNalTypeOffset;
    };

} // h26x

#endif //H26X_UTILS_NALUNIT_H
