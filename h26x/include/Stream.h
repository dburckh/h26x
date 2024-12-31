//
// Created by dburc on 11/30/2024.
//

#ifndef H26X_UTILS_STREAM_H
#define H26X_UTILS_STREAM_H
#include <cstdint>
namespace h26x {
    class Stream {
    public:
        [[nodiscard]] virtual size_t position() const = 0;
        [[nodiscard]] virtual size_t available() const = 0;
        virtual size_t skip(size_t s) = 0;
    };
}
#endif //H26X_UTILS_STREAM_H
