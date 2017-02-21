#pragma once
#include <blik.h>

#include "blik_integration_openh264-1.6.0.h"
#include <addon/openh264-1.6.0_for_blik/codec/api/svc/codec_api.h>

#include <blik.hpp>

class BaseEncoderH264
{
public:
    typedef void (*EncodeFrameCB)(const SFrameBSInfo& frameInfo);

public:
    BaseEncoderH264(sint32 width, sint32 height);
    ~BaseEncoderH264();
    id_share Encode(const uint32* rgba, bool chunking, uint64 ms, id_share aac);

private:
    ISVCEncoder* mEncoder;
    uint08s mFrame;
    SSourcePicture mPic;
    SFrameBSInfo mInfo;
    uint64 mBeginTimeMs;
};
