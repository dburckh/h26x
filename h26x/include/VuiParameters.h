//
// Created by dburc on 11/14/2024.
//

#ifndef USB_CAMERA_VIEWER_VUIPARAMETERS_H
#define USB_CAMERA_VIEWER_VUIPARAMETERS_H
#define EXTENDED_SAR 255

#include <vector>
#include <memory>
#include "BitStream.h"
#include "Writable.h"

namespace h26x {
    class VuiParameters:public Writable {
        class AspectRatio:public Writable {
        public:
            explicit AspectRatio(uint8_t aspectRatioIdc);
            void write(RWBitStream *bitWriter) const override;

            uint8_t aspectRationIdc;
        };
        class AspectRatioExt:public AspectRatio {
        public:
            explicit AspectRatioExt(uint8_t aspectRatioIdc, BitStream *bitstream);
            void write(RWBitStream *bitWriter) const override;

            uint16_t sarWidth;
            uint16_t sarHeight;
        };
        class ColourDescription:public Writable {
        public:
            explicit ColourDescription(BitStream *bitstream);
            void write(RWBitStream *bitWriter) const override;

            uint8_t colourPrimaries;
            uint8_t transferCharacteristics;
            uint8_t matrixCoefficients;
        };
        class VideoSignalType:public Writable {
        public:
            explicit VideoSignalType(BitStream *bitstream);
            void write(RWBitStream *bitWriter) const override;

            uint8_t videoFormat;
            bool videoFullRange;
            ColourDescription *colourDescription;
            ~VideoSignalType();
        };
        class ChromaLoc:public Writable {
        public:
            explicit ChromaLoc(BitStream *bitstream);
            void write(RWBitStream *bitWriter) const override;

            /**
             * [0-5]
             */
            uint8_t typeTopField;
            /**
             * [0-5]
             */
            uint8_t typeBottomField;
        };
        class Timing:public Writable {
        public:
            explicit Timing(BitStream *bitstream);
            void write(RWBitStream *bitWriter) const override;

            uint32_t numUnitsInTick;
            uint32_t timeScale;
            bool fixedFrameRateFlag;
        };
        struct CodedPictureBuffer {
            uint32_t bitRateValueMinus1;
            uint32_t cpbSizeValueMinus1;
            bool cbrFlag;
        };
        class HrdParameters:public Writable {
        public:
            explicit HrdParameters(BitStream *bitstream);
            void write(RWBitStream *bitWriter) const override;

            uint8_t bitRateScale; // u(4)
            uint8_t cpbSizeScale; // u(4)
            std::vector<CodedPictureBuffer> cpb;
            uint8_t initialCpbRemovalDelayLengthMinus1; // u(5)
            uint8_t cpbRemovalDelayLengthMinus1; // u(5)
            uint8_t dpbOutputDelayLengthMinus1; // u(5)
            uint8_t timeOffsetLength; // u(5)
        };
        class BitstreamRestriction:public Writable {
        public:
            explicit BitstreamRestriction(BitStream *bitstream);
            void write(RWBitStream *bitWriter) const override;

            bool motionVectorsOverPicBoundariesFlag;
            /**
             * [0-16]
             */
            uint8_t maxBytesPerPicDenom;
            /**
             * [0-16]
             */
            uint8_t maxBitsPerMbDenom;
            /**
             * [0-16]
             */
            uint8_t log2maxMvLengthHorizontal;
            /**
             * [0-16]
             */
            uint8_t log2maxMvLengthVertical;
            uint16_t numReorderFrames;
            uint16_t maxDecFrameBuffering;
        };
    public:
        explicit VuiParameters(BitStream *bitstream);
        void write(RWBitStream *bitWriter) const override;

        std::unique_ptr<AspectRatio> aspectRatio;
        bool overscanInfoPresentFlag;
        std::unique_ptr<VideoSignalType> videoSignalType;
        std::unique_ptr<ChromaLoc> chromaLoc;
        std::unique_ptr<Timing> timing;
        std::unique_ptr<HrdParameters> nalHrdParameters;
        std::unique_ptr<HrdParameters> vclHrdParameters;
        bool lowDelayHrdFlag; // Only valid if nalHrdParameters || vclHrdParameters
        bool picStructPresentFlag;
        std::unique_ptr<BitstreamRestriction> bitstreamRestriction;
        bool getOverscanAppropriateFlag() const;
        void setOverscanAppropriateFlag(bool v);
        void clearOverscanAppropriateFlag();

    private:
        bool overscanAppropriateFlag;
    };
}

#endif //USB_CAMERA_VIEWER_VUIPARAMETERS_H
