//
// Created by dburc on 11/30/2024.
//

#ifndef H26X_UTILS_COMMON_H
#define H26X_UTILS_COMMON_H

#define SPS_DATA \
{0x64, 0x00, 0x1F, 0xAC, 0x72, 0x84, 0x40, \
0xA0, 0x2F, 0xF9, 0x70, 0x11, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00,  \
0x03, 0x00, 0x3C, 0x0F, 0x18, 0x31, 0x84, 0x60};                                     \

#define PPS_DATA {0xE8, 0x43, 0x94, 0xB2, 0x2C};
//#define PPS_DATA2 {0x68, 0xE8, 0x43, 0x94, 0xB2, 0x2C};

#define IDR_SLICE_DATA \
        {0x88, 0x81, 0x00, 0x06, 0xFF, 0x64, 0x12, 0x78, 0xB0, 0x4D, 0x8F, 0x38, \
        0x26, 0x30, 0x36, 0x01, 0x89, 0xD4, 0x00, 0xA7, 0xB1, 0x8F, 0xEA, 0x05, \
        0xF4, 0x71, 0x40, 0x16, 0x87, 0x5E, 0x6E, 0xF1}; \

#define NIDR_SLICE_NAL {0x00, 0x00, 0x00, 0x01, 0x41, 0x9A, 0x29, 0x44, 0xDA, 0xB4, 0xFD, 0x32, 0x61, 0x4C, 0x02};


#endif //H26X_UTILS_COMMON_H
