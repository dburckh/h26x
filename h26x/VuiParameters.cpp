//
// Created by dburc on 11/14/2024.
//

#include "include/VuiParameters.h"
#include "ExpGolomb.h"

namespace h26x {

    VuiParameters::AspectRatio::AspectRatio(uint8_t aspectRatioIdc) : aspectRationIdc(
            aspectRatioIdc) {
    }

    void VuiParameters::AspectRatio::write(BitStream &bs) const {
        bs.set(aspectRationIdc, 8);
    }

    VuiParameters::AspectRatioExt::AspectRatioExt(uint8_t aspectRatioIdc, BitStream &bs) :
            AspectRatio(aspectRatioIdc) {
        sarWidth = bs.get<uint16_t>(16);
        sarHeight = bs.get<uint16_t>(16);
    }

    void VuiParameters::AspectRatioExt::write(BitStream &bs) const {
        AspectRatio::write(bs);
        bs.set(sarWidth, 16);
        bs.set(sarHeight, 16);
    }

    VuiParameters::VideoSignalType::VideoSignalType(BitStream &bs) {
        videoFormat = bs.get<uint8_t>(3);
        videoFullRange = bs.get();
        // colour_description_present_flag
        colourDescription = bs.get() ? new ColourDescription(bs) : nullptr;
    }

    VuiParameters::VideoSignalType::~VideoSignalType() {
        delete colourDescription;
    }

    void VuiParameters::VideoSignalType::write(BitStream &bs) const {
        bs.set(videoFormat, 3);
        bs.set(videoFullRange);
        Writable::setFlagAndWrite(bs, colourDescription);
    }

    VuiParameters::ColourDescription::ColourDescription(BitStream &bs) {
        colourPrimaries = bs.get<uint8_t>(8);
        transferCharacteristics = bs.get<uint8_t>(8);
        matrixCoefficients = bs.get<uint8_t>(8);
    }

    void VuiParameters::ColourDescription::write(BitStream &bs) const {
        bs.set(colourPrimaries, 8);
        bs.set(transferCharacteristics, 8);
        bs.set(matrixCoefficients, 8);
    }

    VuiParameters::ChromaLoc::ChromaLoc(BitStream &bs) {
        typeTopField = ExpGolomb::get(bs);
        typeBottomField = ExpGolomb::get(bs);
    }

    void VuiParameters::ChromaLoc::write(BitStream &bs) const {
        ExpGolomb::set(typeTopField, bs);
        ExpGolomb::set(typeBottomField, bs);
    }

    VuiParameters::Timing::Timing(BitStream &bs) {
        numUnitsInTick = bs.get<uint32_t>(32);
        timeScale = bs.get<uint32_t>(32);
        fixedFrameRateFlag = bs.get();
    }

    void VuiParameters::Timing::write(BitStream &bs) const {
        bs.set(numUnitsInTick, 32);
        bs.set(timeScale, 32);
        bs.set(fixedFrameRateFlag);
    }

    VuiParameters::HrdParameters::HrdParameters(BitStream &bs) {
        // cpb_cnt_minus1
        auto count = ExpGolomb::get(bs) + 1;
        cpb.reserve(count);
        bitRateScale = bs.get<uint8_t>(4);
        cpbSizeScale = bs.get<uint8_t>(4);
        for (int i=0;i<count;i++) {
            cpb.push_back(CodedPictureBuffer{ExpGolomb::get(bs), ExpGolomb::get(bs), bs.get()});
        }
        initialCpbRemovalDelayLengthMinus1 = bs.get<uint8_t>(5);
        cpbRemovalDelayLengthMinus1 = bs.get<uint8_t>(5);
        dpbOutputDelayLengthMinus1 = bs.get<uint8_t>(5);
        timeOffsetLength = bs.get<uint8_t>(5);
    }

    void VuiParameters::HrdParameters::write(BitStream &bs) const {
        ExpGolomb::set(cpb.capacity() - 1, bs);
        bs.set(bitRateScale, 4);
        bs.set(cpbSizeScale, 4);
        for (auto &&codedPictureBuffer : cpb) {
            ExpGolomb::set(codedPictureBuffer.bitRateValueMinus1, bs);
            ExpGolomb::set(codedPictureBuffer.cpbSizeValueMinus1, bs);
            bs.set(codedPictureBuffer.cbrFlag);
        }
        bs.set(initialCpbRemovalDelayLengthMinus1, 5);
        bs.set(cpbRemovalDelayLengthMinus1, 5);
        bs.set(dpbOutputDelayLengthMinus1, 5);
        bs.set(timeOffsetLength, 5);
    }

    VuiParameters::BitstreamRestriction::BitstreamRestriction(BitStream &bs) {
        motionVectorsOverPicBoundariesFlag = bs.get();
        maxBytesPerPicDenom = ExpGolomb::get(bs);
        maxBitsPerMbDenom = ExpGolomb::get(bs);
        log2maxMvLengthHorizontal = ExpGolomb::get(bs);
        log2maxMvLengthVertical = ExpGolomb::get(bs);
        numReorderFrames = ExpGolomb::get(bs);
        maxDecFrameBuffering = ExpGolomb::get(bs);
    }

    void VuiParameters::BitstreamRestriction::write(BitStream &bs) const {
        bs.set(motionVectorsOverPicBoundariesFlag);
        ExpGolomb::set(maxBitsPerMbDenom, bs);
        ExpGolomb::set(maxBitsPerMbDenom, bs);
        ExpGolomb::set(log2maxMvLengthHorizontal, bs);
        ExpGolomb::set(log2maxMvLengthVertical, bs);
        ExpGolomb::set(numReorderFrames, bs);
        ExpGolomb::set(maxDecFrameBuffering, bs);
    }

    VuiParameters::VuiParameters(BitStream &bs) {
        //aspect_ratio_info_present_flag
        if (bs.get()) {
            auto aspectRatioIdc = bs.get<uint8_t>(8);
            aspectRatio.reset(
                    aspectRatioIdc == EXTENDED_SAR ? new AspectRatioExt(aspectRatioIdc, bs)
                                                   : new AspectRatio(aspectRatioIdc));
        } else {
            aspectRatio = nullptr;
        }
        overscanInfoPresentFlag = bs.get();
        if (overscanInfoPresentFlag) {
            overscanAppropriateFlag = bs.get();
        } else {
            overscanAppropriateFlag = false;
        }
        //video_signal_type_present_flag
        videoSignalType.reset(bs.get() ? new VideoSignalType(bs) : nullptr);
        //chroma_loc_info_present_flag
        chromaLoc.reset(bs.get() ? new ChromaLoc(bs) : nullptr);
        //timing_info_present_flag
        timing.reset(bs.get() ? new Timing(bs) : nullptr);
        //nal_hrd_parameters_present_flag
        nalHrdParameters.reset(bs.get() ? new HrdParameters(bs) : nullptr);
        //vcl_hrd_parameters_present_flag
        vclHrdParameters.reset(bs.get() ? new HrdParameters(bs) : nullptr);
        lowDelayHrdFlag = (nalHrdParameters || vclHrdParameters) && bs.get();

        picStructPresentFlag = bs.get();
        //bitstream_restriction_flag
        bitstreamRestriction.reset(bs.get() ? new BitstreamRestriction(bs) : nullptr);
    }

    bool VuiParameters::getOverscanAppropriateFlag() const {
        return overscanInfoPresentFlag && overscanAppropriateFlag;
    }

    void VuiParameters::setOverscanAppropriateFlag(bool v) {
        overscanInfoPresentFlag = true;
        overscanAppropriateFlag = v;
    }
    void VuiParameters::clearOverscanAppropriateFlag() {
        overscanAppropriateFlag = overscanInfoPresentFlag = false;
    }
    void VuiParameters::write(BitStream &bs) const {
        Writable::setFlagAndWrite(bs, aspectRatio.get());
        bs.set(overscanInfoPresentFlag);
        if (overscanInfoPresentFlag) {
            bs.set(overscanAppropriateFlag);
        }
        Writable::setFlagAndWrite(bs, videoSignalType.get());
        Writable::setFlagAndWrite(bs, chromaLoc.get());
        Writable::setFlagAndWrite(bs, timing.get());
        Writable::setFlagAndWrite(bs, nalHrdParameters.get());
        Writable::setFlagAndWrite(bs, vclHrdParameters.get());
        if (nalHrdParameters || vclHrdParameters) {
            bs.set(lowDelayHrdFlag);
        }
        bs.set(picStructPresentFlag);
        Writable::setFlagAndWrite(bs, bitstreamRestriction.get());
    }
}
