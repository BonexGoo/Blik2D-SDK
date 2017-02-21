#include <blik.h>
#if BLIK_NEED_ADDON_CURL

#if BLIK_WINDOWS
    #pragma comment(lib, "winmm.lib")
#endif

#pragma warning(disable : 4090)

// 도구준비
#include "blik_integration_rtmpdump-2.4.h"

#include <addon/rtmpdump-2.4_for_blik/librtmp/amf.c>
#include <addon/rtmpdump-2.4_for_blik/librtmp/hashswf.c>
#include <addon/rtmpdump-2.4_for_blik/librtmp/log.c>
#include <addon/rtmpdump-2.4_for_blik/librtmp/parseurl.c>
#include <addon/rtmpdump-2.4_for_blik/librtmp/rtmp.c>

#endif
