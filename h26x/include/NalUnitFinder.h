//
// Created by dburc on 11/23/2024.
//

#ifndef H26X_UTILS_NALUNITFINDER_H
#define H26X_UTILS_NALUNITFINDER_H



#define NAL_NOT_FOUND 255
#include <memory>
#include "KMPSearch.h"
#include "NalUnit.h"

namespace h26x {
    class NalUnitFinder {
    public:
        NalUnitFinder(const uint8_t * pBuffer, size_t size);
        NalUnitFinder(const NalUnitFinder& nalUnitFinder) = default;

        const uint8_t * findPrefix();
        /**
         * This is a faster search, but will miss 24 bit NAL prefixes
         */
        const uint8_t * findPrefix4();

        /**
         * Get a NalUnit.  If we are at a NAL UNIT, it is returned
         * otherwise findPrefix() is called to find one.
         * We will advance to the start of read NAL Unit or end of buffer
         * @return nullptr if not found
         */
        std::unique_ptr<NalUnit> findNalUnit();
        std::unique_ptr<NalUnit> findNalUnit4();
        /**
         * Points to 0x[00] 00 00 01
         * @return prefix pointer or nullptr
         */
        [[nodiscard]] const uint8_t * getPrefixPtr() const;
        /**
         * Points to the NAL Unit Type
         * @return pointer to NAL Unit Type or nullptr
         */
        [[nodiscard]] uint8_t getPrefixSize() const;

        /**
         * @return true if we have reached the end of the buffer
         */
        [[nodiscard]] bool isEnd() const;

        [[nodiscard]] uint8_t getNalUnitType();
    private:
        const uint8_t * findPrefix(const KMPSearch& search);
        std::unique_ptr<NalUnit> findNalUnit(const KMPSearch& search);
        const static uint8_t NAL_HEADER3[3];
        const static uint8_t NAL_HEADER4[4];
        const static KMPSearch NAL3;
        const static KMPSearch NAL4;
        /**
         * Buffer pointer, if findPrefix() was successful this points to the NAL TYPE
         */
        const uint8_t * const mBufferPtr;
        const uint8_t * const mBufferEndPtr;
        const uint8_t * mPrefixPtr{};
    };
}


#endif //H26X_UTILS_NALUNITFINDER_H
