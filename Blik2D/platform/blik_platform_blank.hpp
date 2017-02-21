#pragma once
#include <platform/blik_platform.hpp>
#include <platform/blik_platform_impl.hpp>

#if !defined(BLIK_PLATFORM_QT5) && !defined(BLIK_PLATFORM_COCOS2DX) && !defined(BLIK_PLATFORM_NATIVE)

    #include <service/blik_viewmanager.hpp>

    class ViewAPI
    {
    public:
        ViewAPI() {}
        ~ViewAPI() {}

    public:
        static inline dependency*& CurPainter()
        {static dependency* _ = nullptr; return _;}
        static inline dependency& CurColor()
        {static dependency _; return _;}
    };

#endif
