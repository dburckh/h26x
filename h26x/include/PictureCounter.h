//
// Created by dburc on 11/24/2024.
//

#ifndef H26X_UTILS_PICTURECOUNTER_H
#define H26X_UTILS_PICTURECOUNTER_H
#include <cstdint>
#include "SPS.h"
#include "PPS.h"
#include "NalUnit.h"
#include "SliceHeader.h"

namespace h26x {


    class PictureCounter {
    public:
        /**
         *
         * @param pNalType pointer to the NAL Unit Type byte
         * @param size
         * @return picture count
         */
        uint32_t getPictureCount(const NalUnit &nalUnit, const SPS &sps, const PPS &pps);

        uint32_t ref_pic_order_cnt_msb_ {0};
        uint32_t ref_pic_order_cnt_lsb_ {0};
        int32_t prev_frame_num_ {0};
        int32_t prev_frame_num_offset_ {0};
        bool pending_mmco5_ {false};
    private:
        static bool hasMMCO5(SliceHeader const &slice_hdr, NalUnit const &nalUnit);
    };
}

#endif //H26X_UTILS_PICTURECOUNTER_H
