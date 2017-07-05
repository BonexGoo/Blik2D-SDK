#pragma once

////////////////////////////////////////
// Platform (select one or not)
////////////////////////////////////////
//#define BLIK_PLATFORM_COCOS2DX
#define BLIK_PLATFORM_NATIVE
//#define BLIK_PLATFORM_QT5

////////////////////////////////////////
// AddOn
////////////////////////////////////////
#define BLIK_NEED_ADDON_AAC        0
#define BLIK_NEED_ADDON_ALPR       0
#define BLIK_NEED_ADDON_CURL       0
#define BLIK_NEED_ADDON_GIT        0
#define BLIK_NEED_ADDON_H264       0
#define BLIK_NEED_ADDON_JPG        0
#define BLIK_NEED_ADDON_TENSORFLOW 0
#define BLIK_NEED_ADDON_TESSERACT  0

////////////////////////////////////////
// Detail
////////////////////////////////////////
#define BLIK_NEED_FORCED_FAKEWIN  0
#define BLIK_NEED_FULLSCREEN      (BLIK_IPHONE | BLIK_ANDROID)
#define BLIK_NEED_LOW_DEBUGING    0
#define BLIK_NEED_MAIN            0
#define BLIK_NEED_PLATFORM_FILE   1
#define BLIK_NEED_PROFILE         0
#define BLIK_NEED_RELEASE_TRACE   1
