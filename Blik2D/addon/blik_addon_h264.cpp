#include <blik.h>

#if !defined(BLIK_NEED_ADDON_H264) || (BLIK_NEED_ADDON_H264 != 0 && BLIK_NEED_ADDON_H264 != 1)
    #error BLIK_NEED_ADDON_H264 macro is invalid use
#endif
bool __LINK_ADDON_H264__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BLIK_NEED_ADDON_H264

#include "blik_addon_h264.hpp"

#include <addon/blik_addon.hpp>
#include <format/blik_bmp.hpp>
#include <format/blik_flv.hpp>
#include <platform/blik_platform.hpp>

// 등록과정
namespace BLIK
{
    BLIK_DECLARE_ADDON_FUNCTION(H264, Create, id_h264, sint32, sint32, bool)
    BLIK_DECLARE_ADDON_FUNCTION(H264, Release, void, id_h264)
    BLIK_DECLARE_ADDON_FUNCTION(H264, EncodeTo, void, id_h264, const uint32*, id_flash flash, uint64 timems)

    static autorun Bind_AddOn_H264()
    {
        Core_AddOn_H264_Create() = Customized_AddOn_H264_Create;
        Core_AddOn_H264_Release() = Customized_AddOn_H264_Release;
        Core_AddOn_H264_EncodeTo() = Customized_AddOn_H264_EncodeTo;
        return true;
    }
    static autorun _ = Bind_AddOn_H264();
}

// 구현부
namespace BLIK
{
    id_h264 Customized_AddOn_H264_Create(sint32 width, sint32 height, bool fastmode)
    {
        BaseEncoderH264* NewEncoder = new BaseEncoderH264(width, height, fastmode);
        return (id_h264) NewEncoder;
    }

    void Customized_AddOn_H264_Release(id_h264 h264)
    {
        BaseEncoderH264* OldEncoder = (BaseEncoderH264*) h264;
        delete OldEncoder;
    }

    void Customized_AddOn_H264_EncodeTo(id_h264 h264, const uint32* rgba, id_flash flash, uint64 timems)
    {
        BaseEncoderH264* CurEncoder = (BaseEncoderH264*) h264;
        if(CurEncoder)
            return CurEncoder->EncodeTo(rgba, flash, timems);
    }
}

BaseEncoderH264::BaseEncoderH264(sint32 width, sint32 height, bool fastmode)
{
    mEncoder = nullptr;
    int rv = WelsCreateSVCEncoder(&mEncoder);
    BLIK_ASSERT("WelsCreateSVCEncoder가 실패하였습니다", rv == cmResultSuccess && mEncoder != nullptr);

    unsigned int uiTraceLevel = WELS_LOG_ERROR;
    mEncoder->SetOption(ENCODER_OPTION_TRACE_LEVEL, &uiTraceLevel);

    SEncParamExt param;
    mEncoder->GetDefaultParams(&param);
    param.iUsageType                 = SCREEN_CONTENT_REAL_TIME;
    param.iPicWidth                  = width;                // width of picture in samples
    param.iPicHeight                 = height;               // height of picture in samples
    param.fMaxFrameRate              = 60.0f;                // input frame rate
    param.iTargetBitrate             = 4 * width * height;   // target bitrate desired
    param.iMaxBitrate                = UNSPECIFIED_BIT_RATE;
    param.iRCMode                    = (fastmode)? RC_BITRATE_MODE : RC_QUALITY_MODE;
    param.iTemporalLayerNum          = 3;                    // layer number at temporal level
    param.iSpatialLayerNum           = 1;                    // layer number at spatial level
    param.bEnableDenoise             = 0;                    // denoise control
    param.bEnableBackgroundDetection = 1;                    // background detection control
    param.bEnableAdaptiveQuant       = 1;                    // adaptive quantization control
    param.bEnableFrameSkip           = 1;                    // frame skipping
    param.bEnableLongTermReference   = 0;                    // long term reference control
    param.iLtrMarkPeriod             = 30;
    param.uiIntraPeriod              = 320;                  // period of Intra frame
    param.eSpsPpsIdStrategy          = CONSTANT_ID;
    param.bPrefixNalAddingCtrl       = 0;
    param.iComplexityMode            = (fastmode)? LOW_COMPLEXITY : HIGH_COMPLEXITY;
    param.bSimulcastAVC              = false;
    param.iEntropyCodingModeFlag     = 1;                    // 0:CAVLC  1:CABAC.
    for (int i = 0; i < param.iSpatialLayerNum; i++)
    {
        param.sSpatialLayers[i].uiProfileIdc               = PRO_MAIN;
        param.sSpatialLayers[i].iVideoWidth                = width;
        param.sSpatialLayers[i].iVideoHeight               = height;
        param.sSpatialLayers[i].fFrameRate                 = param.fMaxFrameRate;
        param.sSpatialLayers[i].iSpatialBitrate            = param.iTargetBitrate;
        param.sSpatialLayers[i].iMaxSpatialBitrate         = param.iMaxBitrate;
        param.sSpatialLayers[i].sSliceArgument.uiSliceMode = SM_FIXEDSLCNUM_SLICE;
        param.sSpatialLayers[i].sSliceArgument.uiSliceNum  = 1;
    }
    rv = mEncoder->InitializeExt(&param);
    BLIK_ASSERT("InitWithParam이 실패하였습니다", rv == cmResultSuccess);

    memset(&mPic, 0, sizeof(mPic));
    mPic.iPicWidth = width;
    mPic.iPicHeight = height;
    mPic.iColorFormat = videoFormatI420;
    mPic.iStride[0] = mPic.iPicWidth;
    mPic.iStride[1] = mPic.iStride[2] = mPic.iPicWidth / 2;
    mPic.pData[0] = mFrame.AtDumping(0, width * height * 3 / 2); // I420: 1(Y) + 1/4(U) + 1/4(V)
    mPic.pData[1] = mPic.pData[0] + width * height;
    mPic.pData[2] = mPic.pData[1] + width * height / 4;

    memset(&mInfo, 0, sizeof(mInfo));
}

BaseEncoderH264::~BaseEncoderH264()
{
    if(mEncoder)
    {
        mEncoder->Uninitialize();
        WelsDestroySVCEncoder(mEncoder);
    }
}

static const void* GetBE2(sint32 value)
{
    static uint32 Result;
    Result  = (value >>  8) & 0x000000FF;
    Result |= (value <<  8) & 0x0000FF00;
    return &Result;
}

static const void* GetBE3(sint32 value)
{
    static uint32 Result;
    Result  = (value >> 16) & 0x000000FF;
    Result |= (value >>  0) & 0x0000FF00;
    Result |= (value << 16) & 0x00FF0000;
    return &Result;
}

static const void* GetBE4(sint32 value)
{
    static uint32 Result;
    Result  = (value >> 24) & 0x000000FF;
    Result |= (value >>  8) & 0x0000FF00;
    Result |= (value <<  8) & 0x00FF0000;
    Result |= (value << 24) & 0xFF000000;
    return &Result;
}

static const void* GetBE8_Double(double value)
{
    static uint64 Result;
    uint64 SrcValue = *((uint64*) &value);
    Result  = (SrcValue >> 56) & ox00000000000000FF;
    Result |= (SrcValue >> 40) & ox000000000000FF00;
    Result |= (SrcValue >> 24) & ox0000000000FF0000;
    Result |= (SrcValue >>  8) & ox00000000FF000000;
    Result |= (SrcValue <<  8) & ox000000FF00000000;
    Result |= (SrcValue << 24) & ox0000FF0000000000;
    Result |= (SrcValue << 40) & ox00FF000000000000;
    Result |= (SrcValue << 56) & oxFF00000000000000;
    return &Result;
}

static void WriteTag(uint08s& dst, uint08 type, sint32 timestamp, const uint08s chunk)
{
    dst.AtAdding() = type; // type
    Memory::Copy(dst.AtDumpingAdded(3), GetBE3(chunk.Count()), 3); // datasize
    Memory::Copy(dst.AtDumpingAdded(3), GetBE3(timestamp & 0x00FFFFFF), 3); // timestamp
    dst.AtAdding() = (timestamp & 0xFF000000) >> 24; // timestamp extended
    Memory::Copy(dst.AtDumpingAdded(3), GetBE3(0), 3); // streamid
    Memory::Copy(dst.AtDumpingAdded(chunk.Count()), &chunk[0], chunk.Count());
    Memory::Copy(dst.AtDumpingAdded(4), GetBE4(chunk.Count() + 11), 4); // chunk size match
}

static void WriteScriptDataImpl(uint08s& dst, chars name)
{
    const sint32 Length = blik_strlen(name);
    Memory::Copy(dst.AtDumpingAdded(2), GetBE2(Length), 2); // stringsize
    Memory::Copy(dst.AtDumpingAdded(Length), name, Length); // string
}

static void WriteScriptDataECMA(uint08s& dst, chars name, sint32 value)
{
    WriteScriptDataImpl(dst, name);
    dst.AtAdding() = 0x08; // type: ecma
    Memory::Copy(dst.AtDumpingAdded(4), GetBE4(value), 4); // value
}

static void WriteScriptDataNumber(uint08s& dst, chars name, double value)
{
    WriteScriptDataImpl(dst, name);
    dst.AtAdding() = 0x00; // type: number
    Memory::Copy(dst.AtDumpingAdded(8), GetBE8_Double(value), 8); // value
}

static void WriteScriptDataBoolean(uint08s& dst, chars name, bool value)
{
    WriteScriptDataImpl(dst, name);
    dst.AtAdding() = 0x01; // type: boolean
    dst.AtAdding() = value; // value
}

static void WriteScriptDataString(uint08s& dst, chars name, chars value)
{
    WriteScriptDataImpl(dst, name);
    dst.AtAdding() = 0x02; // type: string
    WriteScriptDataImpl(dst, value);
}

void BaseEncoderH264::EncodeTo(const uint32* rgba, id_flash flash, uint64 timems)
{
    uint08* yplane = mPic.pData[0];
    uint08* uplane = mPic.pData[1];
    uint08* vplane = mPic.pData[2];
    for(sint32 i = 0, iend = mPic.iPicWidth * mPic.iPicHeight; i < iend; ++i)
    {
        Bmp::bitmappixel& color = *((Bmp::bitmappixel*) rgba++);
        *(yplane++) = ((66 * color.r + 129 * color.g + 25 * color.b) >> 8) + 16;
        if(!((i / mPic.iPicWidth) & 1) && !(i & 1))
        {
            *(uplane++) = ((-38 * color.r + -74 * color.g + 112 * color.b) >> 8) + 128;
            *(vplane++) = ((112 * color.r + -94 * color.g + -18 * color.b) >> 8) + 128;
        }
    }

    int rv = mEncoder->EncodeFrame(&mPic, &mInfo);
    BLIK_ASSERT("EncodeFrame이 실패하였습니다", rv == cmResultSuccess);

    static uint08s Chunk;
    if(rv == cmResultSuccess && mInfo.eFrameType != videoFrameTypeSkip)
    {
        for(sint32 i = 0; i < mInfo.iLayerNum; ++i)
        {
            const SLayerBSInfo& CurLayer = mInfo.sLayerInfo[i];
            sint32 BufSize = 0;
            for(sint32 j = 0; j < CurLayer.iNalCount; ++j)
                BufSize += CurLayer.pNalLengthInByte[j];

            // Video태그헤더(5)
            Chunk.SubtractionAll();
            const bool IsInterframe = (CurLayer.eFrameType == videoFrameTypeI);
            const bool IsNALU = (CurLayer.uiLayerType == VIDEO_CODING_LAYER);
            Chunk.AtAdding() = (IsInterframe)? 0x27 : 0x17; // 1_:keyframe, 2_:interframe, _7:AVC
            Chunk.AtAdding() = (IsNALU)? 0x01 : 0x00; // AVC NALU, AVC sequence header
            Memory::Copy(Chunk.AtDumpingAdded(3), GetBE3(0), 3); // CompositionTime

            // 태그데이터
            if(IsNALU)
            {
                Memory::Copy(Chunk.AtDumpingAdded(4), GetBE4(BufSize), 4); // NALU Size
                Memory::Copy(Chunk.AtDumpingAdded(BufSize), CurLayer.pBsBuf, BufSize); // NALU
                Flv::AddChunk(flash, 0x09, &Chunk[0], Chunk.Count(), timems); // video
            }
            else
            {
                BLIK_ASSERT("AVC sequence header는 iNalCount가 2여야만 합니다", CurLayer.iNalCount == 2);
                const sint32 SPSBegin = 4;
                const sint32 SPSEnd = CurLayer.pNalLengthInByte[0];
                const sint32 PPSBegin = SPSEnd + 4;
                const sint32 PPSEnd = SPSEnd + CurLayer.pNalLengthInByte[1];
                Chunk.AtAdding() = 0x01; // Configuration Version
                Chunk.AtAdding() = CurLayer.pBsBuf[5]; // AVCProfile-Indication
                Chunk.AtAdding() = CurLayer.pBsBuf[6]; // AVCProfile-Compatibility
                Chunk.AtAdding() = CurLayer.pBsBuf[7]; // AVCLevel-Indication
                Chunk.AtAdding() = 0xFF; // lengthSizeMinusOne
                // SPS
                Chunk.AtAdding() = 0xE1; // SPS Number
                Memory::Copy(Chunk.AtDumpingAdded(2), GetBE2(SPSEnd - SPSBegin), 2); // Size
                Memory::Copy(Chunk.AtDumpingAdded(SPSEnd - SPSBegin), &CurLayer.pBsBuf[SPSBegin], SPSEnd - SPSBegin);
                // PPS
                Chunk.AtAdding() = 0x01; // PPS Number
                Memory::Copy(Chunk.AtDumpingAdded(2), GetBE2(PPSEnd - PPSBegin), 2); // Size
                Memory::Copy(Chunk.AtDumpingAdded(PPSEnd - PPSBegin), &CurLayer.pBsBuf[PPSBegin], PPSEnd - PPSBegin);
                Flv::AddChunk(flash, 0x09, &Chunk[0], Chunk.Count(), timems); // video
            }
        }
    }
}

#endif
