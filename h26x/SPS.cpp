//
// Created by dburc on 11/12/2024.
//

#include <memory>
#include "include/SPS.h"
#include "ExpGolomb.h"

#include <iostream>

namespace h26x {
    bool SPS::ProfileIdcInfoExt::read(BitStream *br) {
        chromaFormatIdc = ExpGolomb::get(br);
        if (chromaFormatIdc >= 4) {
            return false;
        }
        separateColourPlaneFlag = (chromaFormatIdc == CHROMA_FORMAT_IDC_444) && br->get();
        bitDepthLumaMinus8 = ExpGolomb::get(br);
        if (bitDepthLumaMinus8 >= 7) {
            return false;
        }
        bitDepthChromaMinus8 = ExpGolomb::get(br);
        if (bitDepthChromaMinus8 >= 7) {
            return false;
        }
        qpprimeYzeroTransformBypassFlag = br->get();
        //seq_scaling_matrix_present_flag
        if (br->get()) {
            for (int i=0;i<8;i++) {
                seqScalingMatrix.push_back(readScaleList(br, i < 6 ? 16 : 64));
            }
        }
        return br->isOk();
    }

    std::vector<int> SPS::ProfileIdcInfoExt::readScaleList(BitStream *bitstream, int size) {
        std::vector<int> scaleVector;
        int lastScale = 8;
        int nextScale = 8;
        for (int j = 0; j < size; j++) {
            if (nextScale != 0) {
                int deltaScale = ExpGolomb::getSigned(bitstream);
                nextScale = (lastScale + deltaScale + 256) % 256;
                if (j == 0 && nextScale == 0) {
                    return scaleVector;
                }
            }
            lastScale = nextScale == 0 ? lastScale : nextScale;
            scaleVector.push_back(lastScale);
        }
        return scaleVector;
    }

    void SPS::ProfileIdcInfoExt::write(BitStream *bitWriter) const {
        ExpGolomb::set(chromaFormatIdc, bitWriter);
        if (chromaFormatIdc == CHROMA_FORMAT_IDC_444) {
            bitWriter->set(separateColourPlaneFlag);
        }
        ExpGolomb::set(bitDepthLumaMinus8, bitWriter);
        ExpGolomb::set(bitDepthChromaMinus8, bitWriter);
        bitWriter->set(qpprimeYzeroTransformBypassFlag);
        if (seqScalingMatrix.empty()) {
            bitWriter->set(false);
        } else {
            bitWriter->set(true);
            for (const auto& outer : seqScalingMatrix) {
                for (int v : outer) {
                    ExpGolomb::setSigned(v, bitWriter);
                }
            }
        }
    }

    SPS::PicOrderCountType::PicOrderCountType(uint8_t type):type(type) {}

    void SPS::PicOrderCountType::write(BitStream *bitWriter) const {
        ExpGolomb::set(type, bitWriter);
    }

    SPS::PicOrderCountType0::PicOrderCountType0(BitStream *bitstream): PicOrderCountType(0) {
        log2MaxPicOrderCntLsbMinus4 = ExpGolomb::get(bitstream);
    }

    void SPS::PicOrderCountType0::write(BitStream *bitWriter) const {
        PicOrderCountType::write(bitWriter);
        ExpGolomb::set(log2MaxPicOrderCntLsbMinus4, bitWriter);
    }

    SPS::PicOrderCountType1::PicOrderCountType1(BitStream *bitstream): PicOrderCountType(1) {
        deltaPicOrderAlwaysZeroFlag = bitstream->get();
        offsetForNonRefPic = ExpGolomb::getSigned(bitstream);
        offsetForTopToBottomField = ExpGolomb::getSigned(bitstream);
        auto numRefFramesInPicOrderCntCycle = ExpGolomb::get(bitstream);
        expectedDeltaPerPicOrderCntCycle = 0;
        for (int i = 0; i < numRefFramesInPicOrderCntCycle; i++) {
            auto offset = ExpGolomb::getSigned(bitstream);
            offsetForRefFrame.push_back(offset);
            expectedDeltaPerPicOrderCntCycle += offset;
        }
    }

    void SPS::PicOrderCountType1::write(BitStream *bitWriter) const {
        PicOrderCountType::write(bitWriter);
        bitWriter->set(deltaPicOrderAlwaysZeroFlag);
        ExpGolomb::setSigned(offsetForNonRefPic, bitWriter);
        ExpGolomb::setSigned(offsetForTopToBottomField, bitWriter);
        ExpGolomb::set(offsetForRefFrame.size(), bitWriter);
        for (auto &&offset : offsetForRefFrame) {
            ExpGolomb::setSigned(offset, bitWriter);
        }
    }

    SPS::FrameCropOffset::FrameCropOffset(BitStream *bitstream) {
        left = ExpGolomb::get(bitstream);
        right = ExpGolomb::get(bitstream);
        top = ExpGolomb::get(bitstream);
        bottom = ExpGolomb::get(bitstream);
    }

    void SPS::FrameCropOffset::write(BitStream *bitWriter) const {
        ExpGolomb::set(left, bitWriter);
        ExpGolomb::set(right, bitWriter);
        ExpGolomb::set(top, bitWriter);
        ExpGolomb::set(bottom, bitWriter);
    }

    bool SPS::read(BitStream *br) {
        profileIdc = br->get<uint8_t>(8);
        constraintSet0Flag = br->get();
        constraintSet1Flag = br->get();
        constraintSet2Flag = br->get();
        constraintSet3Flag = br->get();
        constraintSet4Flag = br->get();
        constraintSet5Flag = br->get();
        reserved = br->get<uint8_t>(2);
        levelIdc = br->get<uint8_t>(8);
        seqParameterSetId = ExpGolomb::get(br);
        if (profileIdc == 100
            || profileIdc == 110
            || profileIdc == 122
            || profileIdc == 244
            || profileIdc == 44
            || profileIdc == 83
            || profileIdc == 86
            || profileIdc == 118
            || profileIdc == 128
            || profileIdc == 138) {
            auto profileIdcInfoExt = new ProfileIdcInfoExt();
            profileIdcInfo = std::unique_ptr<ProfileIdcInfo>(profileIdcInfoExt);
            profileIdcInfoExt->read(br);
        } else {
            profileIdcInfo = std::make_unique<ProfileIdcInfo>();
        }
        log2MaxFrameNumMinus4 = ExpGolomb::get(br);
        switch (ExpGolomb::get(br)) {
            case 0:
                picOrderCountType = std::make_unique<PicOrderCountType0>(br);
                break;
            case 1:
                picOrderCountType = std::make_unique<PicOrderCountType1>(br);
                break;
            case 2:
                picOrderCountType = std::make_unique<PicOrderCountType>(2);
                break;
            default:
                picOrderCountType = nullptr;
        }
        numRefFrames = ExpGolomb::get(br);
        gapsInFrameNumValueAllowedFlag = br->get();
        picWidthInMbsMinus1 = ExpGolomb::get(br);
        picHeightInMapUnitsMinus1 = ExpGolomb::get(br);
        frameMbsOnlyFlag = br->get();
        mbAdaptiveFrameFieldFlag = !frameMbsOnlyFlag && br->get();
        direct8x8InferenceFlag = br->get();
        //frame_cropping_flag
        frameCropOffset = br->get() ? std::make_unique<FrameCropOffset>(br) : nullptr;

        //vui_parameters_present_flag
        vuiParameters = br->get() ? std::make_unique<VuiParameters>(br) : nullptr;
        //rbsp_stop_one_bit
        bool rbsp_stop_one_bit = br->get(); // Should be true
        return br->isOk();
    }

    void SPS::write(BitStream *bitWriter) const {
        bitWriter->set(profileIdc, 8);
        bitWriter->set(constraintSet0Flag);
        bitWriter->set(constraintSet1Flag);
        bitWriter->set(constraintSet2Flag);
        bitWriter->set(constraintSet3Flag);
        bitWriter->set(constraintSet4Flag);
        bitWriter->set(constraintSet5Flag);
        bitWriter->set(reserved, 2);
        bitWriter->set(levelIdc, 8);
        ExpGolomb::set(seqParameterSetId, bitWriter);
        auto profileIdcExt = dynamic_cast<ProfileIdcInfoExt *>(profileIdcInfo.get());
        if (profileIdcExt) {
            profileIdcExt->write(bitWriter);
        }
        ExpGolomb::set(log2MaxFrameNumMinus4, bitWriter);
        if (picOrderCountType) {
            picOrderCountType->write(bitWriter);
        }
        ExpGolomb::set(numRefFrames, bitWriter);
        bitWriter->set(gapsInFrameNumValueAllowedFlag);
        ExpGolomb::set(picWidthInMbsMinus1, bitWriter);
        ExpGolomb::set(picHeightInMapUnitsMinus1, bitWriter);
        bitWriter->set(frameMbsOnlyFlag);
        if (!frameMbsOnlyFlag) {
            bitWriter->set(mbAdaptiveFrameFieldFlag);
        }
        bitWriter->set(direct8x8InferenceFlag);
        Writable::setFlagAndWrite(bitWriter, frameCropOffset.get());
        Writable::setFlagAndWrite(bitWriter, vuiParameters.get());
        //rbsp_stop_one_bit
        bitWriter->set(true);
        bitWriter->setTrailingBits(false);
    }

    SPS::PicOrderCountType0 *SPS::getPicOrderCountType0() const {
        return dynamic_cast<PicOrderCountType0*>(picOrderCountType.get());
    }
    SPS::PicOrderCountType1 *SPS::getPicOrderCountType1() const {
        return dynamic_cast<PicOrderCountType1*>(picOrderCountType.get());
    }

    uint8_t SPS::getChromaArrayType() const {
        auto profileIdcInfoExt =dynamic_cast<ProfileIdcInfoExt*>(profileIdcInfo.get());
        if (profileIdcInfoExt && profileIdcInfoExt->separateColourPlaneFlag) {
            return 0;
        }
        return profileIdcInfo->chromaFormatIdc;
    }
}

