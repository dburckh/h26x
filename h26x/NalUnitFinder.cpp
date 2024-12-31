//
// Created by dburc on 11/23/2024.
//

#include "NalUnitFinder.h"

namespace h26x {
    NalUnitFinder::NalUnitFinder(const uint8_t *pBuffer, size_t size) :
            mpBuffer(pBuffer),
            mpBufferEnd(pBuffer + size) {
    }

    bool NalUnitFinder::findPrefix() {
        return findPrefix(NAL3);
    }

    bool NalUnitFinder::findPrefix4() {
        return findPrefix(NAL4);
    }

    bool NalUnitFinder::findPrefix(const KMPSearch &search) {
        mpPrefix = search.find(mpBuffer, mpBufferEnd - mpBuffer);
        if (mpPrefix) {
            mpBuffer = mpPrefix + search.needleLen;
            if (search.needleLen == 3 && mpPrefix > mpBuffer && mpPrefix[-1] == 0) {
                mpPrefix--;
            }
            if (isNalType()) {
                return true;
            } else {
                mpPrefix = nullptr;
            }
        } else {
            mpBuffer = mpBufferEnd;
        }
        return false;
    }

    NalUnit * NalUnitFinder::findNalUnit(const KMPSearch &search) {
        if (mpPrefix == nullptr) {
            if (!findPrefix(search)) {
                return nullptr;
            }
        }
        NalUnitFinder prior = *this;
        auto pNalEnd = findPrefix(search) ? mpPrefix : mpBufferEnd;
        return new NalUnit(prior.mpPrefix, pNalEnd - prior.mpPrefix, search.needleLen);
    }

    NalUnit * NalUnitFinder::findNalUnit() {
        return findNalUnit(NAL3);
    }

    NalUnit *NalUnitFinder::findNalUnit4() {
        return findNalUnit(NAL4);
    }

    bool NalUnitFinder::isEnd() const {
        return mpBuffer == mpBufferEnd ;
    }

    bool NalUnitFinder::isNalType() {
        return (*mpBuffer & 0x80) == 0;
    }

    const uint8_t *NalUnitFinder::getPrefixPointer() const {
        return mpPrefix;
    }

    const uint8_t *NalUnitFinder::getNalUnitTypePointer() const {
        if (mpPrefix) {
            return mpBuffer;
        }
        return nullptr;
    }

    const uint8_t NalUnitFinder::NAL_HEADER3[3] = {0,0,1};
    const uint8_t NalUnitFinder::NAL_HEADER4[4] = {0,0,0,1};
    const KMPSearch NalUnitFinder::NAL3 = KMPSearch(NAL_HEADER3 , 3);
    const KMPSearch NalUnitFinder::NAL4 = KMPSearch(NAL_HEADER4, 4);

}