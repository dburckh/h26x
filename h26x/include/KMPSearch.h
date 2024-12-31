//
// Created by dburc on 9/14/2022.
//

#ifndef USB_CAMERA_VIEWER_KMPSEARCH_H
#define USB_CAMERA_VIEWER_KMPSEARCH_H
#include <memory>

namespace h26x {
    class KMPSearch {
    public:
        KMPSearch(uint8_t const *needle, int needleLen);
        const uint8_t * find(const uint8_t* haystack, int haystackLen) const;
        const int needleLen;
    private:
        uint8_t const *mNeedle;
        std::unique_ptr<int[]> mLps;
    };
}
#endif //USB_CAMERA_VIEWER_KMPSEARCH_H
