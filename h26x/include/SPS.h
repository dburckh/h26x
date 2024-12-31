//
// Created by dburc on 11/12/2024.
//

#ifndef USB_CAMERA_VIEWER_SPS_H
#define USB_CAMERA_VIEWER_SPS_H
#define CHROMA_FORMAT_IDC_420 1
#define CHROMA_FORMAT_IDC_444 3
#include <vector>
#include <memory>
#include "BitStream.h"
#include "Writable.h"
#include "VuiParameters.h"

namespace h26x {
    /**
     * Sequence Parameter Set for H264
     */
    class SPS:Writable {
    public:
        SPS() = default;
        class ProfileIdcInfo {
        public:
            /**
             * This is a default
             */
            uint8_t chromaFormatIdc = CHROMA_FORMAT_IDC_420;
            virtual ~ProfileIdcInfo() = default;
        };
        class ProfileIdcInfoExt: public ProfileIdcInfo, Writable {
        public:
            ProfileIdcInfoExt() = default;
            bool read(BitStream *br);
            void write(RWBitStream *bitWriter) const override;
            /**
             * Only for chromaFormatIdc == CHROMA_FORMAT_IDC_444
             */
            bool separateColourPlaneFlag {false};
            uint32_t bitDepthLumaMinus8 {0};
            uint32_t bitDepthChromaMinus8 {0};
            bool qpprimeYzeroTransformBypassFlag {false};
            std::vector< std::vector <int > > seqScalingMatrix;
        private:
            static std::vector<int> readScaleList(BitStream *bitstream, int size);
        };

        class PicOrderCountType:Writable {
        public:
            explicit PicOrderCountType(uint8_t type);
            void write(RWBitStream *bitWriter) const override;
            const uint8_t type;
            virtual ~PicOrderCountType() = default;
        };

        class PicOrderCountType0:public PicOrderCountType {
        public:
            explicit PicOrderCountType0(BitStream *bitstream);
            void write(RWBitStream *bitWriter) const override;
            /**
             * [0-12]
             */
            uint8_t log2MaxPicOrderCntLsbMinus4;
        };

        class PicOrderCountType1:public PicOrderCountType {
        public:
            explicit PicOrderCountType1(BitStream *bitstream);
            void write(RWBitStream *bitWriter) const override;
            bool deltaPicOrderAlwaysZeroFlag;
            int32_t offsetForNonRefPic;
            int32_t offsetForTopToBottomField;
            std::vector<int32_t> offsetForRefFrame;
            int32_t expectedDeltaPerPicOrderCntCycle;
        };

        class FrameCropOffset:public Writable {
        public:
            explicit FrameCropOffset(BitStream *bitstream);
            void write(RWBitStream *bitWriter) const override;
            uint32_t left;
            uint32_t right;
            uint32_t top;
            uint32_t bottom;
        };

        bool read(BitStream * br);
        void write(RWBitStream *bitWriter) const override;

        [[nodiscard]] PicOrderCountType0 * getPicOrderCountType0() const;
        [[nodiscard]] PicOrderCountType1 * getPicOrderCountType1() const;

        uint8_t getChromaArrayType() const;

        uint8_t profileIdc {0};
        bool constraintSet0Flag {false};
        bool constraintSet1Flag {false};
        bool constraintSet2Flag {false};
        bool constraintSet3Flag {false};
        bool constraintSet4Flag {false};
        bool constraintSet5Flag {false};
        uint8_t reserved {0};
        uint8_t levelIdc {0};
        std::unique_ptr<ProfileIdcInfo> profileIdcInfo;
        /**
         * [0-31]
         */
        uint8_t seqParameterSetId {0};
        /**
         * [0-12]
         */
        uint8_t log2MaxFrameNumMinus4 {0};
        // not null
        std::unique_ptr<PicOrderCountType> picOrderCountType;
        uint32_t numRefFrames {0};
        bool gapsInFrameNumValueAllowedFlag {false};
        uint32_t picWidthInMbsMinus1 {0};
        uint32_t picHeightInMapUnitsMinus1 {0};
        bool frameMbsOnlyFlag {false};
        bool mbAdaptiveFrameFieldFlag {false};
        bool direct8x8InferenceFlag {false};
        std::unique_ptr<FrameCropOffset> frameCropOffset;
        std::unique_ptr<VuiParameters> vuiParameters;
    };
}

#endif //USB_CAMERA_VIEWER_SPS_H
