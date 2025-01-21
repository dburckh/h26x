//
// Created by dburc on 11/12/2024.
//

#include <memory>
#include "include/SPS.h"
#include "ExpGolomb.h"

#include <iostream>

namespace h26x {
    bool SPS::ProfileIdcInfoExt::read(BitStream &bs) {
        chromaFormatIdc = ExpGolomb::get(bs);
        if (chromaFormatIdc >= 4) {
            return false;
        }
        separateColourPlaneFlag = (chromaFormatIdc == CHROMA_FORMAT_IDC_444) && bs.get();
        bitDepthLumaMinus8 = ExpGolomb::get(bs);
        if (bitDepthLumaMinus8 >= 7) {
            return false;
        }
        bitDepthChromaMinus8 = ExpGolomb::get(bs);
        if (bitDepthChromaMinus8 >= 7) {
            return false;
        }
        qpprimeYzeroTransformBypassFlag = bs.get();
        //seq_scaling_matrix_present_flag
        if (bs.get()) {
            for (int i=0;i<8;i++) {
                seqScalingMatrix.push_back(readScaleList(bs, i < 6 ? 16 : 64));
            }
        }
        return bs.isOk();
    }

    std::vector<int> SPS::ProfileIdcInfoExt::readScaleList(BitStream &bs, int size) {
        std::vector<int> scaleVector;
        int lastScale = 8;
        int nextScale = 8;
        for (int j = 0; j < size; j++) {
            if (nextScale != 0) {
                int deltaScale = ExpGolomb::getSigned(bs);
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

    void SPS::ProfileIdcInfoExt::write(BitStream &bs) const {
        ExpGolomb::set(chromaFormatIdc, bs);
        if (chromaFormatIdc == CHROMA_FORMAT_IDC_444) {
            bs.set(separateColourPlaneFlag);
        }
        ExpGolomb::set(bitDepthLumaMinus8, bs);
        ExpGolomb::set(bitDepthChromaMinus8, bs);
        bs.set(qpprimeYzeroTransformBypassFlag);
        if (seqScalingMatrix.empty()) {
            bs.set(false);
        } else {
            bs.set(true);
            for (const auto& outer : seqScalingMatrix) {
                for (int v : outer) {
                    ExpGolomb::setSigned(v, bs);
                }
            }
        }
    }

    SPS::PicOrderCountType::PicOrderCountType(uint8_t type):type(type) {}

    void SPS::PicOrderCountType::write(BitStream &bs) const {
        ExpGolomb::set(type, bs);
    }

    SPS::PicOrderCountType0::PicOrderCountType0(BitStream &bs): PicOrderCountType(0) {
        log2MaxPicOrderCntLsbMinus4 = ExpGolomb::get(bs);
    }

    void SPS::PicOrderCountType0::write(BitStream &bs) const {
        PicOrderCountType::write(bs);
        ExpGolomb::set(log2MaxPicOrderCntLsbMinus4, bs);
    }

    SPS::PicOrderCountType1::PicOrderCountType1(BitStream &bs): PicOrderCountType(1) {
        deltaPicOrderAlwaysZeroFlag = bs.get();
        offsetForNonRefPic = ExpGolomb::getSigned(bs);
        offsetForTopToBottomField = ExpGolomb::getSigned(bs);
        auto numRefFramesInPicOrderCntCycle = ExpGolomb::get(bs);
        expectedDeltaPerPicOrderCntCycle = 0;
        for (int i = 0; i < numRefFramesInPicOrderCntCycle; i++) {
            auto offset = ExpGolomb::getSigned(bs);
            offsetForRefFrame.push_back(offset);
            expectedDeltaPerPicOrderCntCycle += offset;
        }
    }

    void SPS::PicOrderCountType1::write(BitStream &bs) const {
        PicOrderCountType::write(bs);
        bs.set(deltaPicOrderAlwaysZeroFlag);
        ExpGolomb::setSigned(offsetForNonRefPic, bs);
        ExpGolomb::setSigned(offsetForTopToBottomField, bs);
        ExpGolomb::set(offsetForRefFrame.size(), bs);
        for (auto &&offset : offsetForRefFrame) {
            ExpGolomb::setSigned(offset, bs);
        }
    }

    SPS::FrameCropOffset::FrameCropOffset(BitStream &bs) {
        left = ExpGolomb::get(bs);
        right = ExpGolomb::get(bs);
        top = ExpGolomb::get(bs);
        bottom = ExpGolomb::get(bs);
    }

    void SPS::FrameCropOffset::write(BitStream &bs) const {
        ExpGolomb::set(left, bs);
        ExpGolomb::set(right, bs);
        ExpGolomb::set(top, bs);
        ExpGolomb::set(bottom, bs);
    }

    bool SPS::read(BitStream &bs) {
        profileIdc = bs.get<uint8_t>(8);
        constraintSet0Flag = bs.get();
        constraintSet1Flag = bs.get();
        constraintSet2Flag = bs.get();
        constraintSet3Flag = bs.get();
        constraintSet4Flag = bs.get();
        constraintSet5Flag = bs.get();
        reserved = bs.get<uint8_t>(2);
        levelIdc = bs.get<uint8_t>(8);
        seqParameterSetId = ExpGolomb::get(bs);
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
            profileIdcInfoExt->read(bs);
        } else {
            profileIdcInfo = std::make_unique<ProfileIdcInfo>();
        }
        log2MaxFrameNumMinus4 = ExpGolomb::get(bs);
        switch (ExpGolomb::get(bs)) {
            case 0:
                picOrderCountType = std::make_unique<PicOrderCountType0>(bs);
                break;
            case 1:
                picOrderCountType = std::make_unique<PicOrderCountType1>(bs);
                break;
            case 2:
                picOrderCountType = std::make_unique<PicOrderCountType>(2);
                break;
            default:
                picOrderCountType = nullptr;
        }
        numRefFrames = ExpGolomb::get(bs);
        gapsInFrameNumValueAllowedFlag = bs.get();
        picWidthInMbsMinus1 = ExpGolomb::get(bs);
        picHeightInMapUnitsMinus1 = ExpGolomb::get(bs);
        frameMbsOnlyFlag = bs.get();
        mbAdaptiveFrameFieldFlag = !frameMbsOnlyFlag && bs.get();
        direct8x8InferenceFlag = bs.get();
        //frame_cropping_flag
        frameCropOffset = bs.get() ? std::make_unique<FrameCropOffset>(bs) : nullptr;

        //vui_parameters_present_flag
        vuiParameters = bs.get() ? std::make_unique<VuiParameters>(bs) : nullptr;
        //rbsp_stop_one_bit
        bool rbsp_stop_one_bit = bs.get(); // Should be true
        return bs.isOk();
    }

    void SPS::write(BitStream &bs) const {
        bs.set(profileIdc, 8);
        bs.set(constraintSet0Flag);
        bs.set(constraintSet1Flag);
        bs.set(constraintSet2Flag);
        bs.set(constraintSet3Flag);
        bs.set(constraintSet4Flag);
        bs.set(constraintSet5Flag);
        bs.set(reserved, 2);
        bs.set(levelIdc, 8);
        ExpGolomb::set(seqParameterSetId, bs);
        auto profileIdcExt = dynamic_cast<ProfileIdcInfoExt *>(profileIdcInfo.get());
        if (profileIdcExt) {
            profileIdcExt->write(bs);
        }
        ExpGolomb::set(log2MaxFrameNumMinus4, bs);
        if (picOrderCountType) {
            picOrderCountType->write(bs);
        }
        ExpGolomb::set(numRefFrames, bs);
        bs.set(gapsInFrameNumValueAllowedFlag);
        ExpGolomb::set(picWidthInMbsMinus1, bs);
        ExpGolomb::set(picHeightInMapUnitsMinus1, bs);
        bs.set(frameMbsOnlyFlag);
        if (!frameMbsOnlyFlag) {
            bs.set(mbAdaptiveFrameFieldFlag);
        }
        bs.set(direct8x8InferenceFlag);
        Writable::setFlagAndWrite(bs, frameCropOffset.get());
        Writable::setFlagAndWrite(bs, vuiParameters.get());
        //rbsp_stop_one_bit
        bs.set(true);
        bs.setTrailingBits(false);
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

