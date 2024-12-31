//
// Created by dburc on 11/14/2024.
//

#include "include/VuiParameters.h"
#include "ExpGolomb.h"

namespace h26x {

    VuiParameters::AspectRatio::AspectRatio(uint8_t aspectRatioIdc) : aspectRationIdc(
            aspectRatioIdc) {
    }

    void VuiParameters::AspectRatio::write(RWBitStream *bitWriter) const {
        bitWriter->set(aspectRationIdc, 8);
    }

    VuiParameters::AspectRatioExt::AspectRatioExt(uint8_t aspectRatioIdc, BitStream *bitstream) :
            AspectRatio(aspectRatioIdc) {
        sarWidth = bitstream->get<uint16_t>(16);
        sarHeight = bitstream->get<uint16_t>(16);
    }

    void VuiParameters::AspectRatioExt::write(RWBitStream *bitWriter) const {
        AspectRatio::write(bitWriter);
        bitWriter->set(sarWidth, 16);
        bitWriter->set(sarHeight, 16);
    }

    VuiParameters::VideoSignalType::VideoSignalType(BitStream *bitstream) {
        videoFormat = bitstream->get<uint8_t>(3);
        videoFullRange = bitstream->get();
        // colour_description_present_flag
        colourDescription = bitstream->get() ? new ColourDescription(bitstream) : nullptr;
    }

    VuiParameters::VideoSignalType::~VideoSignalType() {
        delete colourDescription;
    }

    void VuiParameters::VideoSignalType::write(RWBitStream *bitWriter) const {
        bitWriter->set(videoFormat, 3);
        bitWriter->set(videoFullRange);
        Writable::setFlagAndWrite(bitWriter, colourDescription);
    }

    VuiParameters::ColourDescription::ColourDescription(BitStream* bitstream) {
        colourPrimaries = bitstream->get<uint8_t>(8);
        transferCharacteristics = bitstream->get<uint8_t>(8);
        matrixCoefficients = bitstream->get<uint8_t>(8);
    }

    void VuiParameters::ColourDescription::write(RWBitStream *bitWriter) const {
        bitWriter->set(colourPrimaries, 8);
        bitWriter->set(transferCharacteristics, 8);
        bitWriter->set(matrixCoefficients, 8);
    }

    VuiParameters::ChromaLoc::ChromaLoc(BitStream *bitstream) {
        typeTopField = ExpGolomb::get(bitstream);
        typeBottomField = ExpGolomb::get(bitstream);
    }

    void VuiParameters::ChromaLoc::write(RWBitStream *bitWriter) const {
        ExpGolomb::set(typeTopField, bitWriter);
        ExpGolomb::set(typeBottomField, bitWriter);
    }

    VuiParameters::Timing::Timing(BitStream *bitstream) {
        numUnitsInTick = bitstream->get<uint32_t>(32);
        timeScale = bitstream->get<uint32_t>(32);
        fixedFrameRateFlag = bitstream->get();
    }

    void VuiParameters::Timing::write(RWBitStream *bitWriter) const {
        bitWriter->set(numUnitsInTick, 32);
        bitWriter->set(timeScale, 32);
        bitWriter->set(fixedFrameRateFlag);
    }

    VuiParameters::HrdParameters::HrdParameters(BitStream* bitstream) {
        // cpb_cnt_minus1
        auto count = ExpGolomb::get(bitstream) + 1;
        cpb.reserve(count);
        bitRateScale = bitstream->get<uint8_t>(4);
        cpbSizeScale = bitstream->get<uint8_t>(4);
        for (int i=0;i<count;i++) {
            cpb.push_back(CodedPictureBuffer{ExpGolomb::get(bitstream), ExpGolomb::get(bitstream), bitstream->get()});
        }
        initialCpbRemovalDelayLengthMinus1 = bitstream->get<uint8_t>(5);
        cpbRemovalDelayLengthMinus1 = bitstream->get<uint8_t>(5);
        dpbOutputDelayLengthMinus1 = bitstream->get<uint8_t>(5);
        timeOffsetLength = bitstream->get<uint8_t>(5);
    }

    void VuiParameters::HrdParameters::write(RWBitStream *bitWriter) const {
        ExpGolomb::set(cpb.capacity() - 1, bitWriter);
        bitWriter->set(bitRateScale, 4);
        bitWriter->set(cpbSizeScale, 4);
        for (auto &&codedPictureBuffer : cpb) {
            ExpGolomb::set(codedPictureBuffer.bitRateValueMinus1, bitWriter);
            ExpGolomb::set(codedPictureBuffer.cpbSizeValueMinus1, bitWriter);
            bitWriter->set(codedPictureBuffer.cbrFlag);
        }
        bitWriter->set(initialCpbRemovalDelayLengthMinus1, 5);
        bitWriter->set(cpbRemovalDelayLengthMinus1, 5);
        bitWriter->set(dpbOutputDelayLengthMinus1, 5);
        bitWriter->set(timeOffsetLength, 5);
    }

    VuiParameters::BitstreamRestriction::BitstreamRestriction(BitStream *bitstream) {
        motionVectorsOverPicBoundariesFlag = bitstream->get();
        maxBytesPerPicDenom = ExpGolomb::get(bitstream);
        maxBitsPerMbDenom = ExpGolomb::get(bitstream);
        log2maxMvLengthHorizontal = ExpGolomb::get(bitstream);
        log2maxMvLengthVertical = ExpGolomb::get(bitstream);
        numReorderFrames = ExpGolomb::get(bitstream);
        maxDecFrameBuffering = ExpGolomb::get(bitstream);
    }

    void VuiParameters::BitstreamRestriction::write(RWBitStream *bitWriter) const {
        bitWriter->set(motionVectorsOverPicBoundariesFlag);
        ExpGolomb::set(maxBitsPerMbDenom, bitWriter);
        ExpGolomb::set(maxBitsPerMbDenom, bitWriter);
        ExpGolomb::set(log2maxMvLengthHorizontal, bitWriter);
        ExpGolomb::set(log2maxMvLengthVertical, bitWriter);
        ExpGolomb::set(numReorderFrames, bitWriter);
        ExpGolomb::set(maxDecFrameBuffering, bitWriter);
    }

    VuiParameters::VuiParameters(BitStream *br) {
        //aspect_ratio_info_present_flag
        if (br->get()) {
            auto aspectRatioIdc = br->get<uint8_t>(8);
            aspectRatio.reset(
                    aspectRatioIdc == EXTENDED_SAR ? new AspectRatioExt(aspectRatioIdc, br)
                                                   : new AspectRatio(aspectRatioIdc));
        } else {
            aspectRatio = nullptr;
        }
        overscanInfoPresentFlag = br->get();
        if (overscanInfoPresentFlag) {
            overscanAppropriateFlag = br->get();
        } else {
            overscanAppropriateFlag = false;
        }
        //video_signal_type_present_flag
        videoSignalType.reset(br->get() ? new VideoSignalType(br) : nullptr);
        //chroma_loc_info_present_flag
        chromaLoc.reset(br->get() ? new ChromaLoc(br) : nullptr);
        //timing_info_present_flag
        timing.reset(br->get() ? new Timing(br) : nullptr);
        //nal_hrd_parameters_present_flag
        nalHrdParameters.reset(br->get() ? new HrdParameters(br) : nullptr);
        //vcl_hrd_parameters_present_flag
        vclHrdParameters.reset(br->get() ? new HrdParameters(br) : nullptr);
        lowDelayHrdFlag = (nalHrdParameters || vclHrdParameters) && br->get();

        picStructPresentFlag = br->get();
        //bitstream_restriction_flag
        bitstreamRestriction.reset(br->get() ? new BitstreamRestriction(br) : nullptr);
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
    void VuiParameters::write(RWBitStream *bitWriter) const {
        Writable::setFlagAndWrite(bitWriter, aspectRatio.get());
        bitWriter->set(overscanInfoPresentFlag);
        if (overscanInfoPresentFlag) {
            bitWriter->set(overscanAppropriateFlag);
        }
        Writable::setFlagAndWrite(bitWriter, videoSignalType.get());
        Writable::setFlagAndWrite(bitWriter, chromaLoc.get());
        Writable::setFlagAndWrite(bitWriter, timing.get());
        Writable::setFlagAndWrite(bitWriter, nalHrdParameters.get());
        Writable::setFlagAndWrite(bitWriter, vclHrdParameters.get());
        if (nalHrdParameters || vclHrdParameters) {
            bitWriter->set(lowDelayHrdFlag);
        }
        bitWriter->set(picStructPresentFlag);
        Writable::setFlagAndWrite(bitWriter, bitstreamRestriction.get());
    }
}
