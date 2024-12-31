//
// Created by dburc on 11/21/2024.
//

#ifndef USB_CAMERA_VIEWER_WRITABLE_H
#define USB_CAMERA_VIEWER_WRITABLE_H
#include "RWBitStream.h"

namespace h26x {
    class Writable {
    public:
        virtual void write(RWBitStream *bitWriter) const = 0;

        static void setFlagAndWrite(RWBitStream *bitWriter, Writable * writable) {
            if (writable) {
                bitWriter->set(true);
                writable->write(bitWriter);
            } else {
                bitWriter->set(false);
            }
        };
    };
}
#endif //USB_CAMERA_VIEWER_WRITABLE_H
