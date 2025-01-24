
#include "include/KMPSearch.h"

//
// Created by dburc on 9/12/2022.
//
// Fills lps[] for given pattern pat[0..M-1]
namespace h26x {
    KMPSearch::KMPSearch(const uint8_t *needle, int needleLen):
            needleLen{needleLen},
            mNeedle{needle},
            mLps{new int[needleLen]}{
        // length of the previous longest prefix suffix
        int len = 0;

        mLps[0] = 0; // lps[0] is always 0

        // the loop calculates lps[i] for i = 1 to M-1
        int i = 1;
        while (i < needleLen) {
            if (needle[i] == needle[len]) {
                len++;
                mLps[i] = len;
                i++;
            }
            else {
                // This is tricky. Consider the example.
                // AAACAAAA and i = 7. The idea is similar
                // to findPrefix step.
                if (len != 0) {
                    len = mLps[len - 1];

                    // Also, note that we do not increment
                    // i here
                }
                else {
                    mLps[i] = 0;
                    i++;
                }
            }
        }
    }
    const uint8_t * KMPSearch::find(const uint8_t* const haystack, int haystackLen) const {
        int i = 0; // index for needle[]
        int j = 0; // index for haystack[]
        while ((haystackLen - i) >= (needleLen - j)) {
            if (mNeedle[j] == haystack[i]) {
                j++;
                i++;
            }

            if (j == needleLen) {
                return haystack + i - j;
            }

                // mismatch after j matches
            else if (i < haystackLen && mNeedle[j] != haystack[i]) {
                // Do not match lps[0..lps[j-1]] characters,
                // they will match anyway
                if (j != 0)
                    j = mLps[j - 1];
                else
                    i = i + 1;
            }
        }
        return nullptr;
    }

    KMPSearch::~KMPSearch() {
        delete[] mLps;
    }
}



