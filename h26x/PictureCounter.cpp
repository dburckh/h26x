//
// Created by dburc on 11/24/2024.
//
#include <utility>

#include "include/PictureCounter.h"
#include "include/NalUnitFinder.h"
#include "include/SliceHeader.h"

namespace h26x {
    PictureCounter::PictureCounter(const std::shared_ptr<SPS>& sps, std::shared_ptr<PPS> pps): sps(sps),pps(std::move(pps)),
        mMaxFrameNum( 1 << (sps->log2MaxFrameNumMinus4 + 4)) {
    }
    // Check if a slice includes memory management control operation 5, which
    // results in some |pic_order_cnt| state being cleared.
    bool hasMMCO5(SliceHeader * slice_hdr, NalUnit * nalUnit) {
        // Require that the frame actually has memory management control operations.
        if (nalUnit->getNalRefIdc() == 0 ||
            nalUnit->getH264Type() == NAL_TYPE_IDR ||
            !slice_hdr->adaptive_ref_pic_marking_mode_flag) {
            return false;
        }

        for (auto & i : slice_hdr->ref_pic_marking) {
            int32_t op = i.memory_mgmnt_control_operation;
            if (op == 5)
                return true;

            // Stop at the end of the list.
            if (op == 0)
                return false;
        }

        // Should not get here, the list is always zero terminated.
        return false;
    }

    int PictureCounter::getPictureCount(NalUnit * nalUnit) {
        SliceHeader sliceHeader;
        if (!sliceHeader.read(nalUnit, sps.get(), pps.get())) {
            return -1;
        }
        if (sliceHeader.field_pic_flag) {
            //DLOG(ERROR) << "Interlaced frames are not supported";
            return -1;
        }
        auto idr_pic_flag = nalUnit->getH264Type() == NAL_TYPE_IDR;
        auto nal_ref_idc = nalUnit->getNalRefIdc();
        uint32_t pic_order_cnt = 0;
        bool mmco5 = hasMMCO5(&sliceHeader, nalUnit);

        int32_t max_frame_num = 1 << (sps->log2MaxFrameNumMinus4 + 4);

        // Based on T-REC-H.264 8.2.1, "Decoding process for picture order
        // count", available from http://www.itu.int/rec/T-REC-H.264.
        //
        // Reorganized slightly from spec pseudocode to handle MMCO5 when storing
        // state instead of when loading it.
        //
        // Note: Gaps in frame numbers are ignored. They do not affect POC
        // computation.
        switch (sps->picOrderCountType->type) {
            case 0: {
                auto prev_pic_order_cnt_msb = ref_pic_order_cnt_msb_;
                auto prev_pic_order_cnt_lsb = ref_pic_order_cnt_lsb_;

                // For an IDR picture, clear the state.
                if (idr_pic_flag) {
                    prev_pic_order_cnt_msb = 0;
                    prev_pic_order_cnt_lsb = 0;
                }

                // 8-3. Derive |pic_order_cnt_msb|, accounting for wrapping which is
                //      detected when |pic_order_cnt_lsb| increases or decreases by at
                //      least half of its maximum.
                uint32_t pic_order_cnt_msb;
                auto picOrderCountType0 = sps->getPicOrderCountType0();
                uint32_t max_pic_order_cnt_lsb =
                        1 << (picOrderCountType0->log2MaxPicOrderCntLsbMinus4 + 4);
                if ((sliceHeader.pic_order_cnt_lsb < prev_pic_order_cnt_lsb) &&
                    (prev_pic_order_cnt_lsb - sliceHeader.pic_order_cnt_lsb >=
                     max_pic_order_cnt_lsb / 2)) {
                    pic_order_cnt_msb = prev_pic_order_cnt_msb + max_pic_order_cnt_lsb;
                } else if ((sliceHeader.pic_order_cnt_lsb > prev_pic_order_cnt_lsb) &&
                           (sliceHeader.pic_order_cnt_lsb - prev_pic_order_cnt_lsb >
                            max_pic_order_cnt_lsb / 2)) {
                    pic_order_cnt_msb = prev_pic_order_cnt_msb - max_pic_order_cnt_lsb;
                } else {
                    pic_order_cnt_msb = prev_pic_order_cnt_msb;
                }

                // 8-4, 8-5. Derive |top_field_order_count| and |bottom_field_order_cnt|
                //           (assuming no interlacing).
                auto top_foc = pic_order_cnt_msb + sliceHeader.pic_order_cnt_lsb;
                auto bottom_foc = top_foc + sliceHeader.delta_pic_order_cnt_bottom;

                // Compute POC.
                //
                // MMCO5, like IDR, starts a new reordering group. The POC is specified to
                // change to 0 after decoding; we change it immediately and set the
                // |pending_mmco5_| flag.
                if (mmco5)
                    pic_order_cnt = 0;
                else
                    pic_order_cnt = std::min(top_foc, bottom_foc);

                // Store state.
                pending_mmco5_ = mmco5;
                prev_frame_num_ = sliceHeader.frame_num;
                if (nal_ref_idc != 0) {
                    if (mmco5) {
                        ref_pic_order_cnt_msb_ = 0;
                        ref_pic_order_cnt_lsb_ = top_foc;
                    } else {
                        ref_pic_order_cnt_msb_ = pic_order_cnt_msb;
                        ref_pic_order_cnt_lsb_ = sliceHeader.pic_order_cnt_lsb;
                    }
                }

                break;
            }

            case 1: {
                // 8-6. Derive |frame_num_offset|.
                int32_t frame_num_offset;
                if (idr_pic_flag)
                    frame_num_offset = 0;
                else if (prev_frame_num_ > sliceHeader.frame_num)
                    frame_num_offset = prev_frame_num_offset_ + max_frame_num;
                else
                    frame_num_offset = prev_frame_num_offset_;

                // 8-7. Derive |abs_frame_num|.
                int32_t abs_frame_num;
                auto picOrderCountType1 = sps->getPicOrderCountType1();
                auto num_ref_frames_in_pic_order_cnt_cycle = (int32_t)picOrderCountType1->offsetForRefFrame.size();
                if (num_ref_frames_in_pic_order_cnt_cycle != 0)
                    abs_frame_num = frame_num_offset + sliceHeader.frame_num;
                else
                    abs_frame_num = 0;

                if (nal_ref_idc == 0 && abs_frame_num > 0)
                    abs_frame_num--;

                // 8-9. Derive |expected_pic_order_cnt| (the |pic_order_cnt| indicated
                //      by the cycle described in the SPS).
                int32_t expected_pic_order_cnt = 0;
                if (abs_frame_num > 0) {
                    // 8-8. Derive pic_order_cnt_cycle_cnt and
                    //      frame_num_in_pic_order_cnt_cycle.

                    // H264Parser checks that num_ref_frames_in_pic_order_cnt_cycle < 255.
                    int32_t pic_order_cnt_cycle_cnt =
                            (abs_frame_num - 1) / num_ref_frames_in_pic_order_cnt_cycle;
                    int32_t frame_num_in_pic_order_cnt_cycle =
                            (abs_frame_num - 1) % num_ref_frames_in_pic_order_cnt_cycle;

                    // 8-9 continued.
                    expected_pic_order_cnt = pic_order_cnt_cycle_cnt *
                                             picOrderCountType1->expectedDeltaPerPicOrderCntCycle;
                    for (int32_t i = 0; i <= frame_num_in_pic_order_cnt_cycle; i++)
                        expected_pic_order_cnt += picOrderCountType1->offsetForRefFrame[i];
                }
                if (nal_ref_idc == 0) {
                    expected_pic_order_cnt += picOrderCountType1->offsetForNonRefPic;
                }

                // 8-10. Derive |top_field_order_cnt| and |bottom_field_order_cnt|
                //       (assuming no interlacing).
                int32_t top_foc = expected_pic_order_cnt + sliceHeader.delta_pic_order_cnt0;
                int32_t bottom_foc = top_foc + picOrderCountType1->offsetForTopToBottomField +
                        sliceHeader.delta_pic_order_cnt1;

                // Compute POC. MMCO5 handling is the same as |pic_order_cnt_type| == 0.
                if (mmco5)
                    pic_order_cnt = 0;
                else
                    pic_order_cnt = std::min(top_foc, bottom_foc);

                // Store state.
                pending_mmco5_ = mmco5;
                prev_frame_num_ = sliceHeader.frame_num;
                if (mmco5)
                    prev_frame_num_offset_ = 0;
                else
                    prev_frame_num_offset_ = frame_num_offset;

                break;
            }

            case 2: {
                // 8-11. Derive |frame_num_offset|.
                int32_t frame_num_offset;
                if (idr_pic_flag) {
                    frame_num_offset = 0;
                } else if (prev_frame_num_ > sliceHeader.frame_num) {
                    frame_num_offset = prev_frame_num_offset_ + max_frame_num;
                } else {
                    frame_num_offset = prev_frame_num_offset_;
                }

                // 8-12, 8-13. Derive |temp_pic_order_count| (it's always the
                // |pic_order_cnt|, regardless of interlacing).
                int32_t temp_pic_order_count;
                if (idr_pic_flag)
                    temp_pic_order_count = 0;
                else if (nal_ref_idc == 0)
                    temp_pic_order_count = 2 * (frame_num_offset + sliceHeader.frame_num) - 1;
                else
                    temp_pic_order_count = 2 * (frame_num_offset + sliceHeader.frame_num);

                // Compute POC. MMCO5 handling is the same as |pic_order_cnt_type| == 0.
                if (mmco5)
                    pic_order_cnt = 0;
                else
                    pic_order_cnt = temp_pic_order_count;

                // Store state.
                pending_mmco5_ = mmco5;
                prev_frame_num_ = sliceHeader.frame_num;
                if (mmco5)
                    prev_frame_num_offset_ = 0;
                else
                    prev_frame_num_offset_ = frame_num_offset;

                break;
            }

            default:
                //DLOG(ERROR) << "Invalid pic_order_cnt_type: " << sps->pic_order_cnt_type;
                return -1;
        }

        return pic_order_cnt;
    }
}