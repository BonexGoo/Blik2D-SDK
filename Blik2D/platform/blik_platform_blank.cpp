#include <blik.hpp>
#include "blik_platform_blank.hpp"

#if !defined(BLIK_PLATFORM_QT5) && !defined(BLIK_PLATFORM_COCOS2DX) && !defined(BLIK_PLATFORM_NATIVE)

    #if BLIK_WINDOWS
        #include <windows.h>
    #elif _POSIX_C_SOURCE >= 199309L
        #include <time.h>
    #else
        #include <unistd.h>
    #endif
    #include <stdio.h>
    #include <stdarg.h>
    #if BLIK_ANDROID
        #include <android/asset_manager_jni.h>
    #endif

    dependency* g_data = nullptr;
    dependency* g_window = nullptr;
    dependency* g_view = nullptr;
    dependency* g_func = nullptr;

    #if BLIK_ANDROID
        extern JavaVM* g_JVM;
        jobject g_context = nullptr;
        jobject GetAndroidApplicationContext()
        {
            BLIK_ASSERT("안드로이드OS의 경우에 한하여 ApplicationContext값이 필요합니다", g_context);
            return g_context;
        }
        void SetAndroidApplicationContext(jobject context)
        {
            BLIK_ASSERT("g_JVM가 nullptr입니다", g_JVM);
            JNIEnv* env = nullptr;
            g_JVM->AttachCurrentThread(&env, 0);
            g_context = env->NewGlobalRef(context);
        }
    #endif

    #if BLIK_NEED_MAIN
        extern void PlatformInit();
        extern void PlatformQuit();

        int main(int argc, char* argv[])
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }
    #endif

    #if BLIK_ANDROID
        extern "C" int __android_log_print(int prio, const char *tag,  const char *fmt, ...);
    #endif

    extern "C" int blik_platform_assert(BLIK_DBG_PRM const char* name, const char* query)
    {
        String AssertInfo[4] = {
            String::Format("[QUERY] %s", query),
            String::Format("[METHOD] %s()", BLIK_DBG_FUNC),
            String::Format("[FILE/LINE] %s(%dLn)", BLIK_DBG_FILE, BLIK_DBG_LINE),
            String::Format("[THREAD_ID] -unknown-")};
        #if BLIK_ANDROID
            __android_log_print(7, "*******", "************************************************************");
            __android_log_print(7, "*******", name);
            __android_log_print(7, "*******", "------------------------------------------------------------");
            __android_log_print(7, "*******", (chars) AssertInfo[0]);
            __android_log_print(7, "*******", (chars) AssertInfo[1]);
            __android_log_print(7, "*******", (chars) AssertInfo[2]);
            __android_log_print(7, "*******", (chars) AssertInfo[3]);
            __android_log_print(7, "*******", "************************************************************");
        #elif BLIK_WINDOWS
            OutputDebugStringW(L"************************************************************\n");
            OutputDebugStringW((wchars) WString::FromChars(name));
            OutputDebugStringW(L"\n------------------------------------------------------------\n");
            OutputDebugStringW((wchars) WString::FromChars(AssertInfo[0])); OutputDebugStringW(L"\n");
            OutputDebugStringW((wchars) WString::FromChars(AssertInfo[1])); OutputDebugStringW(L"\n");
            OutputDebugStringW((wchars) WString::FromChars(AssertInfo[2])); OutputDebugStringW(L"\n");
            OutputDebugStringW((wchars) WString::FromChars(AssertInfo[3])); OutputDebugStringW(L"\n");
            OutputDebugStringW(L"************************************************************\n");
        #endif

        if(Platform::Option::GetFlag("AssertPopup"))
        {
            #if BLIK_WINDOWS
                WString AssertMessage = WString::Format(
                    L"%s\n\n%s\t\t\n%s\t\t\n%s\t\t\n%s\t\t\n\n"
                    L"(YES is Break, NO is Ignore)\t\t",
                    (wchars) WString::FromChars(name),
                    (wchars) WString::FromChars(AssertInfo[0]),
                    (wchars) WString::FromChars(AssertInfo[1]),
                    (wchars) WString::FromChars(AssertInfo[2]),
                    (wchars) WString::FromChars(AssertInfo[3]));
                switch(MessageBoxW(NULL, AssertMessage, L"ASSERT BREAK", MB_ICONWARNING | MB_ABORTRETRYIGNORE))
                {
                case IDABORT: return 0;
                case IDIGNORE: return 1;
                }
            #endif
        }
        return 2;
    }

    extern "C" void blik_platform_trace(const char* text, ...)
    {
        char TraceMessage[10240];
        va_list Args;
        va_start(Args, text);
        blik_vsnprintf(TraceMessage, 10240, text, Args);
        va_end(Args);

        #if BLIK_ANDROID
            __android_log_print(7, "#######", TraceMessage);
        #elif BLIK_WINDOWS
            OutputDebugStringW((wchars) WString::FromChars(TraceMessage));
            OutputDebugStringW(L"\n");
        #endif
    }

    namespace BLIK
    {
        ////////////////////////////////////////////////////////////////////////////////
        // PLATFORM
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::InitForGL(bool frameless)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::InitForMDI(bool frameless)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::SetViewCreator(View::CreatorCB creator)
        {
            PlatformImpl::Core::g_Creator = creator;
        }

        void Platform::SetWindowName(chars name)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        h_view Platform::SetWindowView(chars viewclass)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return h_view::null();
        }

        void Platform::SetWindowPos(sint32 x, sint32 y)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::SetWindowSize(sint32 width, sint32 height)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::GetWindowRect(rect128& rect, bool normally)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::SetStatusText(chars text, UIStack stack)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        h_icon Platform::CreateIcon(chars filepath)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return h_icon::null();
        }

        h_action Platform::CreateAction(chars name, chars tip, h_view view, h_icon icon)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return h_action::null();
        }

        h_policy Platform::CreatePolicy(sint32 minwidth, sint32 minheight, sint32 maxwidth, sint32 maxheight)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return h_policy::null();
        }

        h_view Platform::CreateView(chars name, sint32 width, sint32 height, h_policy policy, chars viewclass)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return h_view::null();
        }

        void* Platform::SetNextViewClass(h_view view, chars viewclass)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        bool Platform::SetNextViewManager(h_view view, View* viewmanager)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        h_dock Platform::CreateDock(h_view view, UIAllow allow, UIAllow allowbase)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return h_dock::null();
        }

        void Platform::AddAction(chars group, h_action action, UIRole role)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::AddSeparator(chars group, UIRole role)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::AddToggle(chars group, h_dock dock, UIRole role)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        h_window Platform::OpenChildWindow(h_view view, h_icon icon)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return h_window::null();
        }

        h_window Platform::OpenPopupWindow(h_view view, h_icon icon)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return h_window::null();
        }

        void Platform::CloseWindow(h_window window)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        id_cloned_share Platform::SendNotify(h_view view, chars topic, id_share in, bool needout)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        void Platform::BroadcastNotify(chars topic, id_share in, chars viewclass)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::PassAllViews(PassCB cb, payload data)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // POPUP
        ////////////////////////////////////////////////////////////////////////////////
        bool Platform::Popup::TextDialog(String& text, chars title, chars topic, bool ispassword)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::Popup::ColorDialog(uint08& r, uint08& g, uint08& b, uint08& a, chars title)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::Popup::FileDialog(String& path, String* shortpath, chars title, bool isdir)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);
            return PlatformImpl::Wrap::Popup_FileDialog(path, shortpath, title, isdir);
        }

        void Platform::Popup::WebBrowserDialog(String url)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);
            return PlatformImpl::Wrap::Popup_WebBrowserDialog(url);
        }

        bool Platform::Popup::OpenEditTracker(String& text, UIEditType type, sint32 l, sint32 t, sint32 r, sint32 b)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_view);
            if(r <= l || b <= t) return false;

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        sint32 Platform::Popup::OpenListTracker(Strings textes, sint32 l, sint32 t, sint32 r, sint32 b)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_view);
            if(r <= l || b <= t) return -1;

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return -1;
        }

        void Platform::Popup::CloseAllTracker()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_view);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        bool Platform::Popup::HasOpenedTracker()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_view);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        void Platform::Popup::ShowToolTip(String text)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Popup::HideToolTip()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // UTILITY
        ////////////////////////////////////////////////////////////////////////////////
        static bool NeedSetRandom = true;
        uint32 Platform::Utility::Random()
        {
            if(NeedSetRandom)
            {
                NeedSetRandom = false;
                BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            }

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        void Platform::Utility::Sleep(sint32 ms, bool caninput)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Utility::SetMinimize()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Utility::SetFullScreen()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        bool Platform::Utility::IsFullScreen()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        void Platform::Utility::SetNormalWindow()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Utility::ExitProgram()
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        bool Platform::Utility::GetScreenRect(rect128& rect)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return true;
        }

        id_image_read Platform::Utility::GetScreenshotImage(const rect128& rect)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        id_bitmap_read Platform::Utility::GetScreenshotBitmap(const rect128& rect, bool vflip)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        void Platform::Utility::GetCursorPos(point64& pos)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        sint64 Platform::Utility::CurrentAvailableMemory(sint64* totalbytes)
        {
            return PlatformImpl::Wrap::Utility_CurrentAvailableMemory(totalbytes);
        }

        void Platform::Utility::Threading(ThreadCB cb, payload data)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        uint64 Platform::Utility::CurrentThreadID()
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        uint64 Platform::Utility::CurrentTimeMsec()
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return EpochToWindow(0);
        }

        sint32 Platform::Utility::LastHotKey()
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return -1;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // CLOCK
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::Clock::SetBaseTime(chars timestring)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        id_clock Platform::Clock::Create(sint32 year, sint32 month, sint32 day,
            sint32 hour, sint32 min, sint32 sec, sint64 nsec)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        id_clock Platform::Clock::CreateAsCurrent()
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        id_clock Platform::Clock::CreateAsClone(id_clock_read clock)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        void Platform::Clock::Release(id_clock clock)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        sint64 Platform::Clock::GetPeriodNsec(id_clock_read from, id_clock_read to)
        {
            BLIK_ASSERT("파라미터가 nullptr입니다", from && to);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        void Platform::Clock::AddNsec(id_clock dest, sint64 nsec)
        {
            BLIK_ASSERT("파라미터가 nullptr입니다", dest);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        uint64 Platform::Clock::GetMsec(id_clock clock)
        {
            BLIK_ASSERT("파라미터가 nullptr입니다", clock);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        void Platform::Clock::GetDetail(id_clock clock, sint64* nsec,
            sint32* sec, sint32* min, sint32* hour, sint32* day, sint32* month, sint32* year)
        {
            BLIK_ASSERT("파라미터가 nullptr입니다", clock);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // CLOCK
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::Option::SetFlag(chars name, bool flag)
        {
            PlatformImpl::Wrap::Utility_SetOptionFlag(name, flag);
        }

        bool Platform::Option::GetFlag(chars name)
        {
            return PlatformImpl::Wrap::Utility_GetOptionFlag(name);
        }

        Strings Platform::Option::GetFlagNames()
        {
            return PlatformImpl::Wrap::Utility_GetOptionFlagNames();
        }

        void Platform::Option::SetPayload(chars name, payload data)
        {
            PlatformImpl::Wrap::Utility_SetOptionPayload(name, data);
        }

        payload Platform::Option::GetPayload(chars name)
        {
            return PlatformImpl::Wrap::Utility_GetOptionPayload(name);
        }

        Strings Platform::Option::GetPayloadNames()
        {
            return PlatformImpl::Wrap::Utility_GetOptionPayloadNames();
        }

        ////////////////////////////////////////////////////////////////////////////////
        // GRAPHICS
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::Graphics::SetScissor(float x, float y, float w, float h)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::SetColor(uint08 r, uint08 g, uint08 b, uint08 a)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::SetFont(chars name, float size)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::SetZoom(float zoom)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::EraseRect(float x, float y, float w, float h)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::FillRect(float x, float y, float w, float h)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::FillPolygon(float x, float y, Points p)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::DrawRect(float x, float y, float w, float h, float thick)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::DrawLine(const Point& begin, const Point& end, float thick)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::DrawCircle(float x, float y, float w, float h)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::DrawBezier(const Vector& begin, const Vector& end, float thick)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::DrawPolyLine(float x, float y, Points p, float thick)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            const sint32 Count = p.Count();
            if(Count < 2) return;

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::DrawPolyBezier(float x, float y, Points p, float thick, bool showfirst, bool showlast)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            const sint32 Count = p.Count();
            if(Count < 4) return;

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        id_image Platform::Graphics::CreateImage(id_bitmap_read bitmap, const Color& coloring, sint32 resizing_width, sint32 resizing_height)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        sint32 Platform::Graphics::GetImageWidth(id_image_read image)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetImageHeight(id_image_read image)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        void Platform::Graphics::RemoveImage(id_image image)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::DrawImage(id_image_read image, float ix, float iy, float iw, float ih, float x, float y, float w, float h)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        bool Platform::Graphics::DrawString(float x, float y, float w, float h, chars string, UIFontAlign align, UIFontElide elide)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::Graphics::DrawStringW(float x, float y, float w, float h, wchars string, UIFontAlign align, UIFontElide elide)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        sint32 Platform::Graphics::GetStringWidth(chars string)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringWidthW(wchars string)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringHeight()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringAscent()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        void Platform::Graphics::BeginGL()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::EndGL()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        id_surface Platform::Graphics::CreateSurface(sint32 width, sint32 height)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        sint32 Platform::Graphics::GetSurfaceWidth(id_surface_read surface)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetSurfaceHeight(id_surface_read surface)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        void Platform::Graphics::RemoveSurface(id_surface surface)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::BindSurface(id_surface surface)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::UnbindSurface(id_surface surface)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Graphics::DrawSurface(id_surface_read surface, float sx, float sy, float sw, float sh, float x, float y, float w, float h)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        id_image_read Platform::Graphics::GetImageFromSurface(id_surface_read surface)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        id_bitmap_read Platform::Graphics::GetBitmapFromSurface(id_surface_read surface, bool vflip)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // FILE
        ////////////////////////////////////////////////////////////////////////////////
        bool Platform::File::Exist(chars filename)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::File::ExistForDir(chars dirname)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        id_file_read Platform::File::OpenForRead(chars filename)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        id_file Platform::File::OpenForWrite(chars filename, bool autocreatedir)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        void Platform::File::Close(id_file_read file)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        const sint32 Platform::File::Size(id_file_read file)
        {
            BLIK_ASSERT("해당 파일이 없습니다", file);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        const sint32 Platform::File::Read(id_file_read file, uint08* data, const sint32 size)
        {
            BLIK_ASSERT("해당 파일이 없습니다", file);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        const sint32 Platform::File::ReadLine(id_file_read file, char* text, const sint32 size)
        {
            BLIK_ASSERT("해당 파일이 없습니다", file);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        const sint32 Platform::File::Write(id_file file, bytes data, const sint32 size)
        {
            BLIK_ASSERT("해당 파일이 없습니다", file);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        void Platform::File::Seek(id_file_read file, const sint32 focus)
        {
            BLIK_ASSERT("해당 파일이 없습니다", file);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        const sint32 Platform::File::Focus(id_file_read file)
        {
            BLIK_ASSERT("해당 파일이 없습니다", file);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        sint32 Platform::File::Search(chars dirname, SearchCB cb, payload data, bool needfullpath)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return -1;
        }

        uint32 Platform::File::GetAttributes(wchars itemname, uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        WString Platform::File::GetFullPath(wchars itemname)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname, false));
            if((('A' <= ItemnameUTF8[0] && ItemnameUTF8[0] <= 'Z') ||
                ('a' <= ItemnameUTF8[0] && ItemnameUTF8[0] <= 'z')) && ItemnameUTF8[1] == ':')
                return WString::FromChars(ItemnameUTF8);

            String AbsoluteName;
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);

            #if BLIK_WINDOWS
                return WString::FromChars(AbsoluteName);
            #elif BLIK_MAC_OSX || BLIK_IPHONE
                return WString::FromChars("Q:" + AbsoluteName);
            #elif BLIK_ANDROID
                return WString::FromChars("Q:" + AbsoluteName);
            #endif
        }

        WString Platform::File::GetDirName(wchars itemname, wchar_t badslash, wchar_t goodslash)
        {
            return PlatformImpl::Wrap::File_GetDirName(itemname, badslash, goodslash);
        }

        WString Platform::File::GetShortName(wchars itemname)
        {
            return PlatformImpl::Wrap::File_GetShortName(itemname);
        }

        sint32 Platform::File::GetDriveCode()
        {
            return PlatformImpl::Wrap::File_GetDriveCode();
        }

        bool Platform::File::CanAccess(wchars itemname)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::File::CanWritable(wchars itemname)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::File::Remove(wchars itemname)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::File::Rename(wchars existing_itemname, wchars new_itemname)
        {
            const String ExistingItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(existing_itemname));
            const String NewItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(new_itemname));

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::File::Tempname(char* format, sint32 length)
        {
            return PlatformImpl::Wrap::File_Tempname(format, length);
        }

        bool Platform::File::CreateDir(wchars dirname)
        {
            const String DirnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(dirname));

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::File::RemoveDir(wchars dirname)
        {
            const String DirnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(dirname));

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        sint32 Platform::File::FDOpen(wchars filename, bool writable, bool append, bool exclusive, bool truncate)
        {
            const String FilenameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(filename));

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return -1;
        }

        void Platform::File::FDOpenRetain(sint32 fd)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        bool Platform::File::FDClose(sint32 fd)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        sint64 Platform::File::FDRead(sint32 fd, void* data, sint64 size)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return -1;
        }

        sint64 Platform::File::FDWrite(sint32 fd, const void* data, sint64 size)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return -1;
        }

        sint64 Platform::File::FDSeek(sint32 fd, sint64 offset, sint32 origin)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return -1;
        }

        bool Platform::File::FDResize(sint32 fd, sint64 size)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        void* Platform::File::FDMap(sint32 fd, sint64 offset, sint64 size, bool readonly)
        {
            return PlatformImpl::Wrap::File_FDMap(fd, offset, size, readonly);
        }

        bool Platform::File::FDUnmap(const void* map)
        {
            return PlatformImpl::Wrap::File_FDUnmap(map);
        }

        uint32 Platform::File::FDAttributes(sint32 fd, uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return -1; // INVALID_FILE_ATTRIBUTES
        }

        void Platform::File::ResetAssetsRoot(chars dirname)
        {
            PlatformImpl::Wrap::File_ResetAssetsRoot(dirname);
        }

        void Platform::File::ResetAssetsRemRoot(chars dirname)
        {
            PlatformImpl::Wrap::File_ResetAssetsRemRoot(dirname);
        }

        const String& Platform::File::RootForAssets()
        {
            if(0 < PlatformImpl::Core::g_AssetsRoot.Length())
                return PlatformImpl::Core::g_AssetsRoot;

            #if BLIK_WINDOWS
                PlatformImpl::Core::g_AssetsRoot = "../assets/";
            #elif BLIK_MAC_OSX
                BLIK_ASSERT("Further development is needed.", false);
                PlatformImpl::Core::g_AssetsRoot = "../assets/";
            #elif BLIK_IPHONE
                BLIK_ASSERT("Further development is needed.", false);
                PlatformImpl::Core::g_AssetsRoot = "../assets/";
            #elif BLIK_ANDROID
                PlatformImpl::Core::g_AssetsRoot = "assets:/";
            #else
                PlatformImpl::Core::g_AssetsRoot = "../assets/";
            #endif
            return PlatformImpl::Core::g_AssetsRoot;
        }

        const String& Platform::File::RootForAssetsRem()
        {
            if(0 < PlatformImpl::Core::g_AssetsRemRoot.Length())
                return PlatformImpl::Core::g_AssetsRemRoot;

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            PlatformImpl::Core::g_AssetsRemRoot = "../assets-rem/";
            return PlatformImpl::Core::g_AssetsRemRoot;
        }

        const String& Platform::File::RootForData()
        {
            static String Result = "";
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return Result;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // SOCKET
        ////////////////////////////////////////////////////////////////////////////////
        id_socket Platform::Socket::OpenForTcp()
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        id_socket Platform::Socket::OpenForUdp()
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        bool Platform::Socket::Close(id_socket socket, sint32 timeout)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::Socket::Connect(id_socket socket, chars domain, uint16 port, sint32 timeout)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::Socket::Disconnect(id_socket socket, sint32 timeout)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        sint32 Platform::Socket::RecvAvailable(id_socket socket)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return -1;
        }

        sint32 Platform::Socket::Recv(id_socket socket, uint08* data, sint32 size, sint32 timeout)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return -1;
        }

        sint32 Platform::Socket::Send(id_socket socket, bytes data, sint32 size, sint32 timeout)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return -1;
        }

        class Hostent
        {
        public:
            Hostent() :
                h_addrtype(2), // AF_INET
                h_length(4) // IPv4
            {
                h_name = nullptr;
                h_aliases = nullptr;
                h_addr_list = nullptr;
            }

            ~Hostent()
            {
                delete[] h_name;
                if(h_aliases)
                for(chars* ptr_aliases = h_aliases; *ptr_aliases; ++ptr_aliases)
                    delete[] *ptr_aliases;
                delete[] h_aliases;
                if(h_addr_list)
                for(bytes* ptr_addr_list = h_addr_list; *ptr_addr_list; ++ptr_addr_list)
                    delete[] *ptr_addr_list;
                delete[] h_addr_list;
            }

        public:
            chars h_name;
            chars* h_aliases;
            const sint16 h_addrtype;
            const sint16 h_length;
            bytes* h_addr_list;
        };

        void* Platform::Socket::GetHostByName(chars name)
        {
            static Map<Hostent> HostentMap;
            if(HostentMap.Access(name))
                HostentMap.Remove(name);

            Hostent& CurHostent = HostentMap(name);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return &CurHostent;
        }

        class Servent
        {
        public:
            Servent()
            {
                s_name = nullptr;
                s_aliases = nullptr;
                s_port = 0;
                s_proto = nullptr;
            }

            ~Servent()
            {
                delete[] s_name;
                for(chars* ptr_aliases = s_aliases; ptr_aliases; ++ptr_aliases)
                    delete[] *ptr_aliases;
                delete[] s_aliases;
                delete[] s_proto;
            }

        public:
            chars s_name;
            chars* s_aliases;
            sint16 s_port;
            chars s_proto;
        };

        void* Platform::Socket::GetServByName(chars name, chars proto)
        {
            static Map<Servent> ServentMap;
            Servent& CurServent = ServentMap(name);

            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return &CurServent;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // SERVER
        ////////////////////////////////////////////////////////////////////////////////
        id_server Platform::Server::Create(bool sizefield)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        void Platform::Server::Release(id_server server)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        bool Platform::Server::Listen(id_server server, uint16 port)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::Server::TryNextPacket(id_server server)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        packettype Platform::Server::GetPacketType(id_server server)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return packettype_null;
        }

        sint32 Platform::Server::GetPacketPeerID(id_server server)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return -1;
        }

        bytes Platform::Server::GetPacketBuffer(id_server server, sint32* getsize)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        bool Platform::Server::SendToPeer(id_server server, sint32 peerid, const void* buffer, sint32 buffersize)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // WEB
        ////////////////////////////////////////////////////////////////////////////////
        h_web Platform::Web::Create(chars url, sint32 width, sint32 height, EventCB cb, payload data)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return h_web::null();
        }

        void Platform::Web::Release(h_web web)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Web::Reload(h_web web, chars url)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Web::Resize(h_web web, sint32 width, sint32 height)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Web::SendTouchEvent(h_web web, TouchType type, sint32 x, sint32 y)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        void Platform::Web::SendKeyEvent(h_web web, sint32 code, chars text, bool pressed)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        id_image_read Platform::Web::GetScreenshotImage(h_web web)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        id_bitmap_read Platform::Web::GetScreenshotBitmap(h_web web, bool vflip)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // SERIAL
        ////////////////////////////////////////////////////////////////////////////////
        Strings Platform::Serial::GetAllNames(String* spec)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return Strings();
        }

        id_serial Platform::Serial::Open(chars name, SerialDecodeCB dec, SerialEncodeCB enc)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        void Platform::Serial::Close(id_serial serial)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        bool Platform::Serial::Connected(id_serial serial)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bool Platform::Serial::ReadReady(id_serial serial, sint32* gettype)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        sint32 Platform::Serial::ReadAvailable(id_serial serial)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        sint32 Platform::Serial::Read(id_serial serial, chars format, ...)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        sint32 Platform::Serial::Write(id_serial serial, chars format, ...)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        void Platform::Serial::WriteFlush(id_serial serial, sint32 type)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // CAMERA
        ////////////////////////////////////////////////////////////////////////////////
        Strings Platform::Camera::GetAllNames(String* spec)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return Strings();
        }

        id_camera Platform::Camera::Open(chars name, sint32 width, sint32 height)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        void Platform::Camera::Close(id_camera camera)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        id_camera Platform::Camera::Clone(id_camera camera)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        void Platform::Camera::Capture(id_camera camera, bool preview, bool needstop)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        id_image_read Platform::Camera::LastCapturedImage(id_camera camera, sint32 maxwidth, sint32 maxheight)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        id_bitmap_read Platform::Camera::LastCapturedBitmap(id_camera camera, bool vflip)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        size64 Platform::Camera::LastCapturedSize(id_camera camera)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return {0, 0};
        }

        uint64 Platform::Camera::LastCapturedTimeMS(id_camera camera)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        sint32 Platform::Camera::TotalPictureShotCount(id_camera camera)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        sint32 Platform::Camera::TotalPreviewShotCount(id_camera camera)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // MICROPHONE
        ////////////////////////////////////////////////////////////////////////////////
        Strings Platform::Microphone::GetAllNames(String* spec)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return Strings();
        }

        id_microphone Platform::Microphone::Open(chars name, sint32 maxcount)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }

        void Platform::Microphone::Close(id_microphone microphone)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
        }

        sint32 Platform::Microphone::GetBitRate(id_microphone microphone)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        sint32 Platform::Microphone::GetChannel(id_microphone microphone)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        sint32 Platform::Microphone::GetSampleRate(id_microphone microphone)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return 0;
        }

        bool Platform::Microphone::TryNextSound(id_microphone microphone)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return false;
        }

        bytes Platform::Microphone::GetSoundData(id_microphone microphone, sint32* length, uint64* timems)
        {
            BLIK_ASSERT("This is blank platform. You can use BLIK_PLATFORM_XXX.", false);
            return nullptr;
        }
    }

#endif
