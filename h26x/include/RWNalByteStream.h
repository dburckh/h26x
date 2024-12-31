//
// Created by dburc on 12/13/2024.
//

#ifndef H26X_UTILS_RWNALBYTESTREAM_H
#define H26X_UTILS_RWNALBYTESTREAM_H

#include "RWByteStream.h"
#include "NalByteStream.h"

namespace h26x {

    class RWNalByteStream: public NalByteStream, public RWByteStream {
    public:
        explicit RWNalByteStream(RWByteStream * pByteStream);
        bool write(uint8_t byte) override;

        size_t skip(size_t s) override;
        [[nodiscard]] size_t available() const override;
        [[nodiscard]] size_t position() const override;
        bool read(uint8_t *byte) const override;
    private:
        RWByteStream * mpRWByteStream;
    };

} // h26x

#endif //H26X_UTILS_RWNALBYTESTREAM_H
