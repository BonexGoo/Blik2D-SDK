QT += core
QT += gui
QT += network
QT += opengl
QT += widgets
QT += multimedia
QT += serialport
android: QT += androidextras
QT += webenginewidgets

CONFIG += c++11
CONFIG += mobility
CONFIG += warn_off
MOBILITY += systeminfo
win32-msvc*: QMAKE_CFLAGS += /bigobj
win32-msvc*: QMAKE_CXXFLAGS += /bigobj
android: LIBS += -landroid

INCLUDEPATH += ../../Blik2D/core
INCLUDEPATH += ../../Blik2D
CONFIG(debug, debug|release){
    DEFINES += BLIK_NDEBUG=0
}
CONFIG(release, debug|release){
    DEFINES += BLIK_NDEBUG=1
}

###########################################################
# ADDON
###########################################################
HEADERS += ../../Blik2D/addon/blik_addon.hpp
HEADERS += ../../Blik2D/addon/blik_addon_aac.hpp
HEADERS += ../../Blik2D/addon/blik_addon_alpr.hpp
HEADERS += ../../Blik2D/addon/blik_addon_curl.hpp
HEADERS += ../../Blik2D/addon/blik_addon_git.hpp
HEADERS += ../../Blik2D/addon/blik_addon_h264.hpp
HEADERS += ../../Blik2D/addon/blik_addon_jpg.hpp
HEADERS += ../../Blik2D/addon/blik_addon_tesseract.hpp
HEADERS += ../../Blik2D/addon/blik_fakewin.h
HEADERS += ../../Blik2D/addon/blik_integration_curl-7.51.0.h
HEADERS += ../../Blik2D/addon/blik_integration_fdk-aac-0.1.4.h
HEADERS += ../../Blik2D/addon/blik_integration_libgit2-0.22.2.h
HEADERS += ../../Blik2D/addon/blik_integration_libssh2-1.6.0.h
HEADERS += ../../Blik2D/addon/blik_integration_openalpr-2.3.0.h
HEADERS += ../../Blik2D/addon/blik_integration_opencv-3.1.0.h
HEADERS += ../../Blik2D/addon/blik_integration_opencv-3.1.0_3rdparty_libjpeg.h
HEADERS += ../../Blik2D/addon/blik_integration_openh264-1.6.0.h
HEADERS += ../../Blik2D/addon/blik_integration_openssl-1.1.0c.h
HEADERS += ../../Blik2D/addon/blik_integration_rtmpdump-2.4.h
HEADERS += ../../Blik2D/addon/blik_integration_tesseract-3.04.01.h
SOURCES += ../../Blik2D/addon/blik_addon.cpp
SOURCES += ../../Blik2D/addon/blik_addon_aac.cpp
SOURCES += ../../Blik2D/addon/blik_addon_alpr.cpp
SOURCES += ../../Blik2D/addon/blik_addon_curl.cpp
SOURCES += ../../Blik2D/addon/blik_addon_git.cpp
SOURCES += ../../Blik2D/addon/blik_addon_h264.cpp
SOURCES += ../../Blik2D/addon/blik_addon_jpg.cpp
SOURCES += ../../Blik2D/addon/blik_addon_tesseract.cpp
SOURCES += ../../Blik2D/addon/blik_fakewin.cpp
SOURCES += ../../Blik2D/addon/blik_integration_curl-7.51.0.c
SOURCES += ../../Blik2D/addon/blik_integration_fdk-aac-0.1.4.cpp
SOURCES += ../../Blik2D/addon/blik_integration_libgit2-0.22.2.c
SOURCES += ../../Blik2D/addon/blik_integration_libssh2-1.6.0.c
SOURCES += ../../Blik2D/addon/blik_integration_openalpr-2.3.0.cpp
SOURCES += ../../Blik2D/addon/blik_integration_opencv-3.1.0.cpp
SOURCES += ../../Blik2D/addon/blik_integration_opencv-3.1.0_3rdparty_libjpeg.c
SOURCES += ../../Blik2D/addon/blik_integration_openh264-1.6.0.cpp
SOURCES += ../../Blik2D/addon/blik_integration_openssl-1.1.0c.c
SOURCES += ../../Blik2D/addon/blik_integration_rtmpdump-2.4.c
SOURCES += ../../Blik2D/addon/blik_integration_tesseract-3.04.01.cpp

###########################################################
# CORE
###########################################################
HEADERS += ../../Blik2D/core/blik.h
HEADERS += ../../Blik2D/core/blik.hpp
HEADERS += ../../Blik2D/core/blik_array.hpp
HEADERS += ../../Blik2D/core/blik_asset.hpp
HEADERS += ../../Blik2D/core/blik_buffer.hpp
HEADERS += ../../Blik2D/core/blik_context.hpp
HEADERS += ../../Blik2D/core/blik_counter.hpp
HEADERS += ../../Blik2D/core/blik_library.hpp
HEADERS += ../../Blik2D/core/blik_map.hpp
HEADERS += ../../Blik2D/core/blik_math.hpp
HEADERS += ../../Blik2D/core/blik_memory.hpp
HEADERS += ../../Blik2D/core/blik_mutex.hpp
HEADERS += ../../Blik2D/core/blik_object.hpp
HEADERS += ../../Blik2D/core/blik_parser.hpp
HEADERS += ../../Blik2D/core/blik_profile.hpp
HEADERS += ../../Blik2D/core/blik_queue.hpp
HEADERS += ../../Blik2D/core/blik_remote.hpp
HEADERS += ../../Blik2D/core/blik_share.hpp
HEADERS += ../../Blik2D/core/blik_storage.hpp
HEADERS += ../../Blik2D/core/blik_string.hpp
HEADERS += ../../Blik2D/core/blik_tasking.hpp
HEADERS += ../../Blik2D/core/blik_updater.hpp
HEADERS += ../../Blik2D/core/blik_view.hpp
HEADERS += ../../Blik2D/core/blik_wstring.hpp
SOURCES += ../../Blik2D/core/blik.cpp
SOURCES += ../../Blik2D/core/blik_asset.cpp
SOURCES += ../../Blik2D/core/blik_buffer.cpp
SOURCES += ../../Blik2D/core/blik_context.cpp
SOURCES += ../../Blik2D/core/blik_library.cpp
SOURCES += ../../Blik2D/core/blik_math.cpp
SOURCES += ../../Blik2D/core/blik_memory.cpp
SOURCES += ../../Blik2D/core/blik_mutex.cpp
SOURCES += ../../Blik2D/core/blik_parser.cpp
SOURCES += ../../Blik2D/core/blik_profile.cpp
SOURCES += ../../Blik2D/core/blik_remote.cpp
SOURCES += ../../Blik2D/core/blik_share.cpp
SOURCES += ../../Blik2D/core/blik_storage.cpp
SOURCES += ../../Blik2D/core/blik_string.cpp
SOURCES += ../../Blik2D/core/blik_tasking.cpp
SOURCES += ../../Blik2D/core/blik_updater.cpp
SOURCES += ../../Blik2D/core/blik_view.cpp
SOURCES += ../../Blik2D/core/blik_wstring.cpp

###########################################################
# ELEMENT
###########################################################
HEADERS += ../../Blik2D/element/blik_clip.hpp
HEADERS += ../../Blik2D/element/blik_color.hpp
HEADERS += ../../Blik2D/element/blik_font.hpp
HEADERS += ../../Blik2D/element/blik_image.hpp
HEADERS += ../../Blik2D/element/blik_matrix.hpp
HEADERS += ../../Blik2D/element/blik_point.hpp
HEADERS += ../../Blik2D/element/blik_rect.hpp
HEADERS += ../../Blik2D/element/blik_size.hpp
HEADERS += ../../Blik2D/element/blik_tween.hpp
HEADERS += ../../Blik2D/element/blik_vector.hpp
SOURCES += ../../Blik2D/element/blik_clip.cpp
SOURCES += ../../Blik2D/element/blik_color.cpp
SOURCES += ../../Blik2D/element/blik_font.cpp
SOURCES += ../../Blik2D/element/blik_image.cpp
SOURCES += ../../Blik2D/element/blik_matrix.cpp
SOURCES += ../../Blik2D/element/blik_point.cpp
SOURCES += ../../Blik2D/element/blik_rect.cpp
SOURCES += ../../Blik2D/element/blik_size.cpp
SOURCES += ../../Blik2D/element/blik_tween.cpp
SOURCES += ../../Blik2D/element/blik_vector.cpp

###########################################################
# FORMAT
###########################################################
HEADERS += ../../Blik2D/format/blik_bmp.hpp
HEADERS += ../../Blik2D/format/blik_png.hpp
HEADERS += ../../Blik2D/format/blik_zen.hpp
SOURCES += ../../Blik2D/format/blik_bmp.cpp
SOURCES += ../../Blik2D/format/blik_png.cpp
SOURCES += ../../Blik2D/format/blik_zen.cpp

###########################################################
# PLATFORM
###########################################################
HEADERS += ../../Blik2D/platform/blik_platform.hpp
HEADERS += ../../Blik2D/platform/blik_platform_blank.hpp
HEADERS += ../../Blik2D/platform/blik_platform_cocos2dx.hpp
HEADERS += ../../Blik2D/platform/blik_platform_impl.hpp
HEADERS += ../../Blik2D/platform/blik_platform_native.hpp
HEADERS += ../../Blik2D/platform/blik_platform_qt5.hpp
SOURCES += ../../Blik2D/platform/blik_platform_blank.cpp
SOURCES += ../../Blik2D/platform/blik_platform_cocos2dx.cpp
SOURCES += ../../Blik2D/platform/blik_platform_impl.cpp
SOURCES += ../../Blik2D/platform/blik_platform_native.cpp
SOURCES += ../../Blik2D/platform/blik_platform_qt5.cpp
HEADERS += ../../Blik2D/platform/android/OpenGLES_Functions.h
SOURCES += ../../Blik2D/platform/android/OpenGLES_Functions.cpp
HEADERS += ../../Blik2D/platform/win32/glew.h

###########################################################
# SERVICE
###########################################################
HEADERS += ../../Blik2D/service/blik_asyncfilemanager.hpp
HEADERS += ../../Blik2D/service/blik_batchbuilder.hpp
HEADERS += ../../Blik2D/service/blik_boxr.hpp
HEADERS += ../../Blik2D/service/blik_brii.hpp
HEADERS += ../../Blik2D/service/blik_oauth2service.hpp
HEADERS += ../../Blik2D/service/blik_zay.hpp
SOURCES += ../../Blik2D/service/blik_asyncfilemanager.cpp
SOURCES += ../../Blik2D/service/blik_batchbuilder.cpp
SOURCES += ../../Blik2D/service/blik_boxr.cpp
SOURCES += ../../Blik2D/service/blik_brii.cpp
SOURCES += ../../Blik2D/service/blik_oauth2service.cpp
SOURCES += ../../Blik2D/service/blik_zay.cpp
