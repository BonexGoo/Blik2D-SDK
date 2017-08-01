TARGET = linecalc
TEMPLATE = app

!include(../../Blik2D/project/blik2d.pri) {
    error("Couldn't find the blik2d.pri file...")
}

INCLUDEPATH += ../source
HEADERS += ../source/blik_config.h
SOURCES += ../source/linecalc.cpp
HEADERS += ../source/linecalc.hpp
SOURCES += ../source/main.cpp
SOURCES += ../source/r.cpp
HEADERS += ../source/r.hpp

ASSETS_IMAGE.files += ../assets/image
ASSETS_IMAGE.path = /assets

win32{
    RC_ICONS += ../common/windows/main.ico
}

android{
    INSTALLS += ASSETS_IMAGE
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../common/android
}

ios|macx{
    QMAKE_BUNDLE_DATA += ASSETS_IMAGE
    QMAKE_BUNDLE_DATA += MAIN_ICON
    QMAKE_INFO_PLIST = $$PWD/../common/ios/Info.plist
    MAIN_ICON.files += $$PWD/../common/ios/res/icon.icns
    ios: MAIN_ICON.path = /
    macx: MAIN_ICON.path = /Contents/Resources
}
