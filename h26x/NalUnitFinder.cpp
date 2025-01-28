//
// Created by dburc on 11/23/2024.
//

#include "NalUnitFinder.h"

namespace h26x {
    NalUnitFinder::NalUnitFinder(const uint8_t *pBuffer, size_t size) :
            mBufferPtr(pBuffer),
            mBufferEndPtr(pBuffer + size) {
    }

    const uint8_t * NalUnitFinder::findPrefix() {
        return findPrefix(NAL3);
    }

    const uint8_t * NalUnitFinder::findPrefix4() {
        return findPrefix(NAL4);
    }

    const uint8_t * NalUnitFinder::findPrefix(const KMPSearch &search) {
        if (isEnd()) {
            return nullptr;
        }
        const auto startPtr = mPrefixPtr ? mPrefixPtr + search.needleLen : mBufferPtr;
        mPrefixPtr = search.find(startPtr, mBufferEndPtr - startPtr);
        if (mPrefixPtr) {
            if (search.needleLen == 3 && mPrefixPtr != mBufferPtr && mPrefixPtr[-1] == 0) {
                mPrefixPtr--;
            }
            return mPrefixPtr;
        } else {
            mPrefixPtr = mBufferEndPtr;
        }
        return nullptr;
    }

    std::unique_ptr<NalUnit> NalUnitFinder::findNalUnit(const KMPSearch &search) {
        if (isEnd()) {
            return nullptr;
        }
        if (mPrefixPtr == nullptr) {
            if (!findPrefix(search)) {
                return nullptr;
            }
        }
        auto startPtr = mPrefixPtr;
        findPrefix(search);
        return std::make_unique<NalUnit>(startPtr, mPrefixPtr - startPtr, search.needleLen);
    }

    std::unique_ptr<NalUnit> NalUnitFinder::findNalUnit() {
        return findNalUnit(NAL3);
    }

    std::unique_ptr<NalUnit> NalUnitFinder::findNalUnit4() {
        return findNalUnit(NAL4);
    }

    bool NalUnitFinder::isEnd() const {
        return mPrefixPtr == mBufferEndPtr ;
    }

    const uint8_t *NalUnitFinder::getPrefixPtr() const {
        return mPrefixPtr;
    }

    uint8_t NalUnitFinder::getPrefixSize() const {
        if (mPrefixPtr) {
            if (mPrefixPtr[2] == 1) {
                return 3;
            } else if (mPrefixPtr[3] == 1) {
                return 4;
            }
        }
        return 0;
    }

    uint8_t NalUnitFinder::getNalUnitType() {
        if (mPrefixPtr) {
            return *(mPrefixPtr + getPrefixSize());
        }
        return 0;
    }

    const uint8_t NalUnitFinder::NAL_HEADER3[3] = {0,0,1};
    const uint8_t NalUnitFinder::NAL_HEADER4[4] = {0,0,0,1};
    const KMPSearch NalUnitFinder::NAL3 = KMPSearch(NAL_HEADER3 , 3);
    const KMPSearch NalUnitFinder::NAL4 = KMPSearch(NAL_HEADER4, 4);

}