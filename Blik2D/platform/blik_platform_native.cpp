#include <blik.h>
#if BLIK_MAC_OSX | BLIK_IPHONE
    #include <CoreFoundation/CoreFoundation.h>
    #include <syslog.h>
#endif

#include <blik.hpp>
#include "blik_platform_native.hpp"

#ifdef BLIK_PLATFORM_NATIVE

    #if BLIK_WINDOWS
        #include <windows.h>
    #elif _POSIX_C_SOURCE >= 199309L
        #include <time.h>
    #else
        #include <unistd.h>
    #endif
    #if !BLIK_WINDOWS
        #include <sys/stat.h>
        #include <dirent.h>
    #endif
    #include <stdio.h>
    #include <stdarg.h>
    #if BLIK_ANDROID
        #include <android/asset_manager_jni.h>
    #endif

    dependency* g_data = nullptr;
    dependency* g_window = nullptr;
    ViewAPI* g_view = nullptr;
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
            BLIK_ASSERT("This application will operate for library only.", false);
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
        #elif BLIK_IPHONE
            syslog(LOG_INFO, "************************************************************\n");
            syslog(LOG_INFO, name);
            syslog(LOG_INFO, "\n------------------------------------------------------------\n");
            syslog(LOG_INFO, (chars) AssertInfo[0]); syslog(LOG_INFO, "\n");
            syslog(LOG_INFO, (chars) AssertInfo[1]); syslog(LOG_INFO, "\n");
            syslog(LOG_INFO, (chars) AssertInfo[2]); syslog(LOG_INFO, "\n");
            syslog(LOG_INFO, (chars) AssertInfo[3]); syslog(LOG_INFO, "\n");
            syslog(LOG_INFO, "************************************************************\n");
        #endif

        if(Platform::Utility::GetOptionFlag("AssertPopup"))
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
            #elif BLIK_IPHONE
                /*SInt32 nRes = 0;
                const void* keys[] = {
                    kCFUserNotificationAlertHeaderKey,
                    kCFUserNotificationAlertMessageKey};
                const void* vals[] = {
                    CFSTR("Test Foundation Message Box"),
                    CFSTR("Hello, World!")};
                CFDictionaryRef dict = CFDictionaryCreate(0, keys, vals,
                    sizeof(keys) / sizeof(*keys),
                    &kCFTypeDictionaryKeyCallBacks,
                    &kCFTypeDictionaryValueCallBacks);
                CFUserNotificationCreate(kCFAllocatorDefault, 0,
                    kCFUserNotificationPlainAlertLevel, &nRes, dict);*/
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
        #elif BLIK_IPHONE
            syslog(LOG_INFO, TraceMessage);
        #endif
    }

    namespace BLIK
    {
        ////////////////////////////////////////////////////////////////////////////////
        // PLATFORM
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::InitForGL()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::InitForMDI()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::SetViewCreator(View::CreatorCB creator)
        {
            PlatformImpl::Core::g_Creator = creator;
        }

        void Platform::SetWindowName(chars name)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
        }

        h_view Platform::SetWindowView(chars viewclass)
        {
            if(g_view)
            {
                Buffer::Free((buffer) g_view);
                g_view = nullptr;
            }

            auto NewViewManager = PlatformImpl::Core::g_Creator(viewclass);
            buffer NewViewAPI = Buffer::AllocNoConstructorOnce<ViewAPI>(BLIK_DBG 1);
            BLIK_CONSTRUCTOR(NewViewAPI, 0, ViewAPI, NewViewManager);

            g_view = (ViewAPI*) NewViewAPI;
            h_view NewViewHandle = h_view::create_by_ptr(BLIK_DBG g_view);
            NewViewManager->SetView(NewViewHandle);
            g_view->sendCreate();
            return NewViewHandle;
        }

        void Platform::SetWindowPos(sint32 x, sint32 y)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::SetWindowSize(sint32 width, sint32 height)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::GetWindowRect(rect128& rect)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
        }

        bool Platform::GetScreenRect(rect128& rect)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return true;
        }

        id_image_read Platform::GetScreenshotImage(const rect128& rect)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_bitmap_read Platform::GetScreenshotBitmap(const rect128& rect, bool vflip)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::SetStatusText(chars text, UIStack stack)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
        }

        h_icon Platform::CreateIcon(chars filepath)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return h_icon::null();
        }

        h_action Platform::CreateAction(chars name, chars tip, h_view view, h_icon icon)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return h_action::null();
        }

        h_policy Platform::CreatePolicy(sint32 minwidth, sint32 minheight, sint32 maxwidth, sint32 maxheight)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return h_policy::null();
        }

        h_view Platform::CreateView(chars name, sint32 width, sint32 height, h_policy policy, chars viewclass)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return h_view::null();
        }

        void* Platform::SetNextViewClass(h_view view, chars viewclass)
        {
            auto NewViewManager = PlatformImpl::Core::g_Creator(viewclass);
            ((ViewAPI*) view.get())->setNextViewManager(NewViewManager);
            return NewViewManager->GetClass();
        }

        bool Platform::SetNextViewManager(h_view view, View* viewmanager)
        {
            ((ViewAPI*) view.get())->setNextViewManager(viewmanager);
            return true;
        }

        h_dock Platform::CreateDock(h_view view, UIAllow allow, UIAllow allowbase)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return h_dock::null();
        }

        void Platform::AddAction(chars group, h_action action, UIRole role)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::AddSeparator(chars group, UIRole role)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::AddToggle(chars group, h_dock dock, UIRole role)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
        }

        h_window Platform::OpenChildWindow(h_view view, h_icon icon)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return h_window::null();
        }

        h_window Platform::OpenPopupWindow(h_view view, h_icon icon)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return h_window::null();
        }

        void Platform::CloseWindow(h_window window)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
        }

        id_cloned_share Platform::SendNotify(h_view view, chars topic, id_share in, bool needout)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::BroadcastNotify(chars topic, id_share in, chars viewclass)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::PassAllViews(PassCB cb, payload data)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // POPUP
        ////////////////////////////////////////////////////////////////////////////////
        bool Platform::Popup::TextDialog(String& text, chars title, chars topic, bool ispassword)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Popup::ColorDialog(uint08& r, uint08& g, uint08& b, uint08& a, chars title)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
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

            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Popup::OpenListTracker(Strings textes, sint32 l, sint32 t, sint32 r, sint32 b)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_view);
            if(r <= l || b <= t) return -1;

            BLIK_ASSERT("Further development is needed.", false);
            return -1;
        }

        void Platform::Popup::CloseAllTracker()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_view);

            BLIK_ASSERT("Further development is needed.", false);
        }

        bool Platform::Popup::HasOpenedTracker()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_view);

            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        void Platform::Popup::ShowToolTip(String text)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Popup::HideToolTip()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data);

            BLIK_ASSERT("Further development is needed.", false);
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
                BLIK_ASSERT("Further development is needed.", false);
            }

            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Utility::ExitProgram()
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        sint64 Platform::Utility::CurrentAvailableMemory(sint64* totalbytes)
        {
            return PlatformImpl::Wrap::Utility_CurrentAvailableMemory(totalbytes);
        }

        void Platform::Utility::Sleep(sint32 ms, bool caninput)
        {
            #if BLIK_WINDOWS
                ::Sleep(ms);
            #elif _POSIX_C_SOURCE >= 199309L
                struct timespec ts;
                ts.tv_sec = ms / 1000;
                ts.tv_nsec = (ms % 1000) * 1000000;
                nanosleep(&ts, NULL);
            #else
                usleep(ms * 1000);
            #endif
        }

        void Platform::Utility::Threading(ThreadCB cb, payload data)
        {
            BLIK_ASSERT("Further development is needed.", false);

            /*#if BLIK_WINDOWS
                #include <windows.h>
                #define THREAD_ID HANDLE
                #define THREAD_CREATE(ID, ARG) do{(ID) = CreateThread(nullptr, 0, _TaskCore, ARG, 0, nullptr);} while(false)
                #define THREAD_FUNCTION(NAME, ARG) static DWORD WINAPI NAME(LPVOID ARG)
                #define THREAD_FUNCTION_RETURN do{return 0;} while(false)
            #else
                #include <pthread.h>
                #define THREAD_ID pthread_t
                #define THREAD_CREATE(ID, ARG) pthread_create(&(ID), nullptr, _TaskCore, ARG)
                #define THREAD_FUNCTION(NAME, ARG) static void* NAME(void* ARG)
                #define THREAD_FUNCTION_RETURN do{return nullptr;} while(false)
            #endif*/
        }

        uint64 Platform::Utility::CurrentThreadID()
        {
            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        uint64 Platform::Utility::CurrentTimeMs()
        {
            #if BLIK_WINDOWS
                FILETIME FileTime;
                GetSystemTimeAsFileTime(&FileTime);
                return *((uint64*) &FileTime) / 10000;
            #elif BLIK_MAC_OSX || BLIK_IPHONE
                return (uint64) (CFAbsoluteTimeGetCurrent() * 1000);
            #elif BLIK_ANDROID
                BLIK_ASSERT("Further development is needed.", false);
                return 0;
            #endif
        }

        void Platform::Utility::SetClockBaseTime(chars timestring)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        id_clock Platform::Utility::CreateCurrentClock()
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_clock Platform::Utility::CreateClonedClock(id_clock_read clock)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Utility::ReleaseClock(id_clock clock)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        sint64 Platform::Utility::GetClockPeriodNsec(id_clock_read from, id_clock_read to)
        {
            BLIK_ASSERT("파라미터가 nullptr입니다", from && to);

            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Utility::AddClockNsec(id_clock dest, sint64 nsec)
        {
            BLIK_ASSERT("파라미터가 nullptr입니다", dest);

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Utility::GetClockDetail(id_clock clock, sint64* nsec,
            sint32* sec, sint32* min, sint32* hour, sint32* day, sint32* month, sint32* year)
        {
            BLIK_ASSERT("파라미터가 nullptr입니다", clock);

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Utility::SetOptionFlag(chars name, bool flag)
        {
            PlatformImpl::Wrap::Utility_SetOptionFlag(name, flag);
        }

        bool Platform::Utility::GetOptionFlag(chars name)
        {
            return PlatformImpl::Wrap::Utility_GetOptionFlag(name);
        }

        Strings Platform::Utility::GetOptionFlagNames()
        {
            return PlatformImpl::Wrap::Utility_GetOptionFlagNames();
        }

        void Platform::Utility::SetOptionPayload(chars name, payload data)
        {
            PlatformImpl::Wrap::Utility_SetOptionPayload(name, data);
        }

        payload Platform::Utility::GetOptionPayload(chars name)
        {
            return PlatformImpl::Wrap::Utility_GetOptionPayload(name);
        }

        Strings Platform::Utility::GetOptionPayloadNames()
        {
            return PlatformImpl::Wrap::Utility_GetOptionPayloadNames();
        }

        ////////////////////////////////////////////////////////////////////////////////
        // GRAPHICS
        ////////////////////////////////////////////////////////////////////////////////
        void Platform::Graphics::SetScissor(float x, float y, float w, float h)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetColor(uint08 r, uint08 g, uint08 b, uint08 a)
        {
            BLIK_ASSERT("Qt5플랫폼처럼 CurCanvas()로의 통합이 필요!!!", false);

            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            ViewAPI::CurColor() = Color(r, g, b, a);
        }

        void Platform::Graphics::SetFont(chars name, float size)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::SetZoom(float zoom)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            ViewAPI::CurPainter()->setZoom(zoom);
        }

        void Platform::Graphics::EraseRect(float x, float y, float w, float h)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::FillRect(float x, float y, float w, float h)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            ViewAPI::CurPainter()->fillRect(Rect(x, y, x + w, y + h), ViewAPI::CurColor());
        }

        void Platform::Graphics::FillPolygon(float x, float y, Points p)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawRect(float x, float y, float w, float h, float thick)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawLine(const Point& begin, const Point& end, float thick)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawCircle(float x, float y, float w, float h)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawBezier(const Vector& begin, const Vector& end, float thick)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawPolyLine(float x, float y, Points p, float thick)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            const sint32 Count = p.Count();
            if(Count < 2) return;

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawPolyBezier(float x, float y, Points p, float thick, bool showfirst, bool showlast)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());
            const sint32 Count = p.Count();
            if(Count < 4) return;

            BLIK_ASSERT("Further development is needed.", false);
        }

        id_image Platform::Graphics::CreateImage(id_bitmap_read bitmap, const Color& coloring, sint32 resizing_width, sint32 resizing_height)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        sint32 Platform::Graphics::GetImageWidth(id_image_read image)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetImageHeight(id_image_read image)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", g_data && g_window);

            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Graphics::RemoveImage(id_image image)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawImage(id_image_read image, float ix, float iy, float iw, float ih, float x, float y, float w, float h)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        bool Platform::Graphics::DrawString(float x, float y, float w, float h, chars string, UIFontAlign align, UIFontElide elide)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Graphics::DrawStringW(float x, float y, float w, float h, wchars string, UIFontAlign align, UIFontElide elide)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Graphics::GetStringWidth(chars string)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringWidthW(wchars string)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringHeight()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetStringAscent()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Graphics::BeginGL()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::EndGL()
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        id_surface Platform::Graphics::CreateSurface(sint32 width, sint32 height)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        sint32 Platform::Graphics::GetSurfaceWidth(id_surface_read surface)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Graphics::GetSurfaceHeight(id_surface_read surface)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Graphics::RemoveSurface(id_surface surface)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::BindSurface(id_surface surface)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::UnbindSurface(id_surface surface)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Graphics::DrawSurface(id_surface_read surface, float sx, float sy, float sw, float sh, float x, float y, float w, float h)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
        }

        id_image_read Platform::Graphics::GetImageFromSurface(id_surface_read surface)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_bitmap_read Platform::Graphics::GetBitmapFromSurface(id_surface_read surface, bool vflip)
        {
            BLIK_ASSERT("호출시점이 적절하지 않습니다", ViewAPI::CurPainter());

            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // FILE
        ////////////////////////////////////////////////////////////////////////////////
        bool Platform::File::Exist(chars filename)
        {
            if(FILE* CheckFile = fopen(filename, "rb"))
            {
                fclose(CheckFile);
                return true;
            }
            return false;
        }

        bool Platform::File::ExistForDir(chars dirname)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        id_file_read Platform::File::OpenForRead(chars filename)
        {
            return (id_file_read) fopen(filename, "rb");
        }

        id_file Platform::File::OpenForWrite(chars filename, bool autocreatedir)
        {
            FILE* WriteFile = fopen(filename, "wb");
            if(autocreatedir && !WriteFile)
            {
                String DirPath = "";
                for(chars iChar = filename; *iChar; ++iChar)
                {
                    const char OneChar = *iChar;
                    if(OneChar == '/' || OneChar == '\\')
                        #if BLIK_WINDOWS
                            CreateDirectoryA(DirPath, nullptr);
                        #else
                            mkdir(DirPath, 0777);
                        #endif
                    DirPath += OneChar;
                }
                WriteFile = fopen(filename, "wb");
            }
            return (id_file) WriteFile;
        }

        void Platform::File::Close(id_file_read file)
        {
            fclose((FILE*) file);
        }

        const sint32 Platform::File::Size(id_file_read file)
        {
            BLIK_ASSERT("해당 파일이 없습니다", file);
            long CurrentPos = ftell((FILE*) file);
            fseek((FILE*) file, 0, SEEK_END);
            long Result = ftell((FILE*) file);
            fseek((FILE*) file, CurrentPos, SEEK_SET);
            return Result;
        }

        const sint32 Platform::File::Read(id_file_read file, uint08* data, const sint32 size)
        {
            BLIK_ASSERT("해당 파일이 없습니다", file);
            return (sint32) fread(data, 1, size, (FILE*) file);
        }

        const sint32 Platform::File::ReadLine(id_file_read file, char* text, const sint32 size)
        {
            BLIK_ASSERT("해당 파일이 없습니다", file);
            return (sint32) fgets(text, size, (FILE*) file);
        }

        const sint32 Platform::File::Write(id_file file, bytes data, const sint32 size)
        {
            BLIK_ASSERT("해당 파일이 없습니다", file);
            return (sint32) fwrite(data, 1, size, (FILE*) file);
        }

        void Platform::File::Seek(id_file_read file, const sint32 focus)
        {
            BLIK_ASSERT("해당 파일이 없습니다", file);
            fseek((FILE*) file, focus, SEEK_SET);
        }

        const sint32 Platform::File::Focus(id_file_read file)
        {
            BLIK_ASSERT("해당 파일이 없습니다", file);
            return (sint32) ftell((FILE*) file);
        }

        sint32 Platform::File::Search(chars dirname, SearchCB cb, payload data, bool needfullpath)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return -1;
        }

        uint32 Platform::File::GetAttributes(wchars itemname, uint64* size, uint64* ctime, uint64* atime, uint64* mtime)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));

            #if BLIK_WINDOWS
                BLIK_ASSERT("Further development is needed.", false);
                return -1;
            #else
                struct stat CurInfo;
                if(lstat(ItemnameUTF8, &CurInfo) == -1)
                    return -1; // INVALID_FILE_ATTRIBUTES

                uint32 Result = 0;
                if(!(CurInfo.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH))) Result |= 0x1; // FILE_ATTRIBUTE_READONLY
                #if !BLIK_ANDROID
                    if(CurInfo.st_flags & UF_HIDDEN) Result |= 0x2; // FILE_ATTRIBUTE_HIDDEN
                #endif
                if(S_ISDIR(CurInfo.st_mode)) Result |= 0x10; // FILE_ATTRIBUTE_DIRECTORY
                if(S_ISLNK(CurInfo.st_mode)) Result |= 0x400; // FILE_ATTRIBUTE_REPARSE_POINT

                if(size) *size = (uint64) CurInfo.st_size;
                #if !BLIK_ANDROID && (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE))
                    if(ctime) *ctime = ((uint64) CurInfo.st_ctimespec.tv_sec) * 1000000000 + CurInfo.st_ctimespec.tv_nsec;
                    if(atime) *atime = ((uint64) CurInfo.st_atimespec.tv_sec) * 1000000000 + CurInfo.st_atimespec.tv_nsec;
                    if(mtime) *mtime = ((uint64) CurInfo.st_mtimespec.tv_sec) * 1000000000 + CurInfo.st_mtimespec.tv_nsec;
                #else
                    if(ctime) *ctime = ((uint64) CurInfo.st_ctime) * 1000000000 + CurInfo.st_ctimensec;
                    if(atime) *atime = ((uint64) CurInfo.st_atime) * 1000000000 + CurInfo.st_atimensec;
                    if(mtime) *mtime = ((uint64) CurInfo.st_mtime) * 1000000000 + CurInfo.st_mtimensec;
                #endif
                return Result;
            #endif
        }

        WString Platform::File::GetFullPath(wchars itemname)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname, false));
            if((('A' <= ItemnameUTF8[0] && ItemnameUTF8[0] <= 'Z') ||
                ('a' <= ItemnameUTF8[0] && ItemnameUTF8[0] <= 'z')) && ItemnameUTF8[1] == ':')
                return WString::FromChars(ItemnameUTF8);

            String AbsoluteName = BLIK::Platform::File::RootForAssetsRem() + String::FromWChars(itemname);
            if(AbsoluteName[-2] == '/' || AbsoluteName[-2] == '\\')
                AbsoluteName.Sub(1);

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

            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::File::CanWritable(wchars itemname)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));

            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::File::Remove(wchars itemname)
        {
            const String ItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(itemname));

            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::File::Rename(wchars existing_itemname, wchars new_itemname)
        {
            const String ExistingItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(existing_itemname));
            const String NewItemnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(new_itemname));

            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::File::Tempname(char* format, sint32 length)
        {
            return PlatformImpl::Wrap::File_Tempname(format, length);
        }

        bool Platform::File::CreateDir(wchars dirname)
        {
            const String DirnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(dirname));

            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::File::RemoveDir(wchars dirname)
        {
            const String DirnameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(dirname));

            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::File::FDOpen(wchars filename, bool writable, bool append, bool exclusive, bool truncate)
        {
            const String FilenameUTF8 = String::FromWChars(PlatformImpl::Core::NormalPathW(filename));

            BLIK_ASSERT("Further development is needed.", false);
            return -1;
        }

        void Platform::File::FDOpenRetain(sint32 fd)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        bool Platform::File::FDClose(sint32 fd)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        sint64 Platform::File::FDRead(sint32 fd, void* data, sint64 size)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return -1;
        }

        sint64 Platform::File::FDWrite(sint32 fd, const void* data, sint64 size)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return -1;
        }

        sint64 Platform::File::FDSeek(sint32 fd, sint64 offset, sint32 origin)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return -1;
        }

        bool Platform::File::FDResize(sint32 fd, sint64 size)
        {
            BLIK_ASSERT("Further development is needed.", false);
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
            BLIK_ASSERT("Further development is needed.", false);
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
                PlatformImpl::Core::g_AssetsRoot = CFStringGetCStringPtr(CFURLGetString(
                    CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle())), kCFStringEncodingUTF8) + 7; // 7은 file://
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

            #if BLIK_WINDOWS
                String NewRoot = "../assets-rem";
                CreateDirectoryA(NewRoot, nullptr);
                NewRoot += "/";
                PlatformImpl::Core::g_AssetsRemRoot = NewRoot;
            #elif BLIK_MAC_OSX
                BLIK_ASSERT("Further development is needed.", false);
                PlatformImpl::Core::g_AssetsRemRoot = "../assets-rem/";
            #elif BLIK_IPHONE
                String NewRoot = CFStringGetCStringPtr(
                    CFURLGetString(CFCopyHomeDirectoryURL()), kCFStringEncodingUTF8) + 7; // 7은 file://
                NewRoot += "Library";
                if(access(NewRoot, 0777)) mkdir(NewRoot, 0777);
                NewRoot += "/Caches";
                if(access(NewRoot, 0777)) mkdir(NewRoot, 0777);
                NewRoot += "/assets-rem";
                if(access(NewRoot, 0777)) mkdir(NewRoot, 0777);
                NewRoot += "/";
                PlatformImpl::Core::g_AssetsRemRoot = NewRoot;
            #elif BLIK_ANDROID
                BLIK_ASSERT("Further development is needed.", false);
                PlatformImpl::Core::g_AssetsRemRoot = "../assets-rem/";
            #else
                PlatformImpl::Core::g_AssetsRemRoot = "../assets-rem/";
            #endif
            return PlatformImpl::Core::g_AssetsRemRoot;
        }

        const String& Platform::File::RootForData()
        {
            static String Result = "";
            BLIK_ASSERT("Further development is needed.", false);
            return Result;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // SOCKET
        ////////////////////////////////////////////////////////////////////////////////
        id_socket Platform::Socket::OpenForTcp()
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_socket Platform::Socket::OpenForUdp()
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        bool Platform::Socket::Close(id_socket socket, sint32 timeout)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Socket::Connect(id_socket socket, chars domain, uint16 port, sint32 timeout)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Socket::Disconnect(id_socket socket, sint32 timeout)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Socket::RecvAvailable(id_socket socket)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return -1;
        }

        sint32 Platform::Socket::Recv(id_socket socket, uint08* data, sint32 size, sint32 timeout)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return -1;
        }

        sint32 Platform::Socket::Send(id_socket socket, bytes data, sint32 size, sint32 timeout)
        {
            BLIK_ASSERT("Further development is needed.", false);
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

            BLIK_ASSERT("Further development is needed.", false);
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

            BLIK_ASSERT("Further development is needed.", false);
            return &CurServent;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // SERVER
        ////////////////////////////////////////////////////////////////////////////////
        id_server Platform::Server::Create(bool sizefield)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Server::Release(id_server server)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        bool Platform::Server::Listen(id_server server, uint16 port)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Server::TryNextPacket(id_server server)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        packettype Platform::Server::GetPacketType(id_server server)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return packettype_null;
        }

        sint32 Platform::Server::GetPacketPeerID(id_server server)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return -1;
        }

        bytes Platform::Server::GetPacketBuffer(id_server server, sint32* getsize)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        bool Platform::Server::SendToPeer(id_server server, sint32 peerid, const void* buffer, sint32 buffersize)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // WEB
        ////////////////////////////////////////////////////////////////////////////////
        h_web Platform::Web::Create(chars url, sint32 width, sint32 height, EventCB cb, payload data)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return h_web::null();
        }

        void Platform::Web::Release(h_web web)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Web::Reload(h_web web, chars url)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Web::Resize(h_web web, sint32 width, sint32 height)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Web::SendTouchEvent(h_web web, TouchType type, sint32 x, sint32 y)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        void Platform::Web::SendKeyEvent(h_web web, sint32 code, chars text, bool pressed)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        id_image_read Platform::Web::GetScreenshotImage(h_web web)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_bitmap_read Platform::Web::GetScreenshotBitmap(h_web web, bool vflip)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // SERIAL
        ////////////////////////////////////////////////////////////////////////////////
        Strings Platform::Serial::GetAllNames(String* spec)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return Strings();
        }

        id_serial Platform::Serial::Open(chars name, SerialDecodeCB dec, SerialEncodeCB enc)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Serial::Close(id_serial serial)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        bool Platform::Serial::Connected(id_serial serial)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        bool Platform::Serial::ReadReady(id_serial serial, sint32* gettype)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        sint32 Platform::Serial::ReadAvailable(id_serial serial)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Serial::Read(id_serial serial, chars format, ...)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Serial::Write(id_serial serial, chars format, ...)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        void Platform::Serial::WriteFlush(id_serial serial, sint32 type)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // CAMERA
        ////////////////////////////////////////////////////////////////////////////////
        Strings Platform::Camera::GetAllNames(String* spec)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return Strings();
        }

        id_camera Platform::Camera::Open(chars name, sint32 width, sint32 height)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Camera::Close(id_camera camera)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        id_camera Platform::Camera::Clone(id_camera camera)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Camera::Capture(id_camera camera, bool preview, bool needstop)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        id_image_read Platform::Camera::LastCapturedImage(id_camera camera, sint32 maxwidth, sint32 maxheight)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        id_bitmap_read Platform::Camera::LastCapturedBitmap(id_camera camera, bool vflip)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        size64 Platform::Camera::LastCapturedSize(id_camera camera)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return {0, 0};
        }

        uint64 Platform::Camera::LastCapturedTimeMS(id_camera camera)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Camera::TotalPictureShotCount(id_camera camera)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Camera::TotalPreviewShotCount(id_camera camera)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // MICROPHONE
        ////////////////////////////////////////////////////////////////////////////////
        Strings Platform::Microphone::GetAllNames(String* spec)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return Strings();
        }

        id_microphone Platform::Microphone::Open(chars name, sint32 maxcount)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }

        void Platform::Microphone::Close(id_microphone microphone)
        {
            BLIK_ASSERT("Further development is needed.", false);
        }

        sint32 Platform::Microphone::GetBitRate(id_microphone microphone)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Microphone::GetChannel(id_microphone microphone)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        sint32 Platform::Microphone::GetSampleRate(id_microphone microphone)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return 0;
        }

        bool Platform::Microphone::TryNextSound(id_microphone microphone)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return false;
        }

        bytes Platform::Microphone::GetSoundData(id_microphone microphone, sint32* length, uint64* timems)
        {
            BLIK_ASSERT("Further development is needed.", false);
            return nullptr;
        }
    }

    NativePainter::NativePainter()
    {
        switch(__version())
        {
        case 0x20: initShaderES20(); break;
        case 0x30: initShaderES30(); break;
        default:
            BLIK_ASSERT("지원되는 OpenGL버전이 아닙니다", false);
            m_program = 0;
            m_vshader = 0;
            m_fshader = 0;
            break;
        }
        m_width = 0;
        m_height = 0;
    }

    NativePainter::~NativePainter()
    {
        termShaderES();
    }

    void NativePainter::reset(const sint32 width, const sint32 height)
    {
        m_width = width * 2; // 현 SPINE좌표계랑 맞추기 위한 방편
        m_height = height * 2; // 차후 수정요망
        setZoom(1);
    }

    void NativePainter::setZoom(const float zoom)
    {
        __loadIdentity();
        // 4평면만 사용하기에 zoom이 2배가 되어야 함
        const float w = zoom * +2 / m_width;
        const float h = zoom * -2 / m_height;
        __multiply(
            w, 0, 0, 0,
            0, h, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);
    }

    void NativePainter::fillRect(const BLIK::Rect& rect, const BLIK::Color& color)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 1); __testGL(BLIK_DBG 0);
        // 편리한 좌표계를 위해 4평면만 사용
        glViewport(-m_width, 0, m_width * 2, m_height * 2); __testGL(BLIK_DBG 0);

        glUseProgram(m_program); __testGL(BLIK_DBG 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0); __testGL(BLIK_DBG 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); __testGL(BLIK_DBG 0);
        glEnableVertexAttribArray(VerticeID); __testGL(BLIK_DBG 0);
        glEnableVertexAttribArray(ColorID); __testGL(BLIK_DBG 0);
        glVertexAttribPointer(VerticeID, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &m_attrib[0].vertices[0]); __testGL(BLIK_DBG 0);
        glVertexAttribPointer(ColorID, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Attrib), &m_attrib[0].colors[0]); __testGL(BLIK_DBG 0);
        glUniformMatrix4fv(m_matrix, 1, GL_FALSE, (const GLfloat*) &m_m[0][0]); __testGL(BLIK_DBG 0);

        glDisable(GL_CULL_FACE); __testGL(BLIK_DBG 0);
        glDisable(GL_DEPTH_TEST); __testGL(BLIK_DBG 0);
        glDisable(GL_SCISSOR_TEST); __testGL(BLIK_DBG 0);
        glEnable(GL_BLEND); __testGL(BLIK_DBG 0);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); __testGL(BLIK_DBG 0);

        m_attrib[0].vertices[0] = rect.l;
        m_attrib[0].vertices[1] = rect.t;
        m_attrib[0].color32 = color.rgba;
        m_attrib[1].vertices[0] = rect.r;
        m_attrib[1].vertices[1] = rect.t;
        m_attrib[1].color32 = color.rgba;
        m_attrib[2].vertices[0] = rect.l;
        m_attrib[2].vertices[1] = rect.b;
        m_attrib[2].color32 = color.rgba;
        m_attrib[3].vertices[0] = rect.r;
        m_attrib[3].vertices[1] = rect.b;
        m_attrib[3].color32 = color.rgba;
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); __testGL(BLIK_DBG 0);
    }

    void NativePainter::initShaderES(chars vsource, chars fsource)
    {
        static chars VertexShaderSourceCodePtr = vsource;
        m_vshader = glCreateShader(GL_VERTEX_SHADER); __testGL(BLIK_DBG 0);
        glShaderSource(m_vshader, 1, &VertexShaderSourceCodePtr, NULL); __testGL(BLIK_DBG 0);
        glCompileShader(m_vshader); __testShader(BLIK_DBG m_vshader);

        static chars FragmentShaderSourceCodePtr = fsource;
        m_fshader = glCreateShader(GL_FRAGMENT_SHADER); __testGL(BLIK_DBG 0);
        glShaderSource(m_fshader, 1, &FragmentShaderSourceCodePtr, NULL); __testGL(BLIK_DBG 0);
        glCompileShader(m_fshader); __testShader(BLIK_DBG m_fshader);

        m_program = glCreateProgram(); __testGL(BLIK_DBG 0);
        glAttachShader(m_program, m_vshader); __testShader(BLIK_DBG m_vshader);
        glAttachShader(m_program, m_fshader); __testShader(BLIK_DBG m_fshader);

        glEnableVertexAttribArray(VerticeID); __testGL(BLIK_DBG 0);
        glEnableVertexAttribArray(ColorID); __testGL(BLIK_DBG 0);
        glBindAttribLocation(m_program, VerticeID, "a_position"); __testGL(BLIK_DBG 0);
        glBindAttribLocation(m_program, ColorID, "a_color"); __testGL(BLIK_DBG 0);
        glVertexAttribPointer(VerticeID, 2, GL_FLOAT, GL_FALSE, sizeof(Attrib), &m_attrib[0].vertices[0]); __testGL(BLIK_DBG 0);
        glVertexAttribPointer(ColorID, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Attrib), &m_attrib[0].colors[0]); __testGL(BLIK_DBG 0);

        glLinkProgram(m_program); __testProgram(BLIK_DBG m_program);
        glValidateProgram(m_program); __testProgram(BLIK_DBG m_program);

        __loadIdentity();
        glUseProgram(m_program); __testProgram(BLIK_DBG m_program);
        m_matrix = glGetUniformLocation(m_program, "u_matrix"); __testGL(BLIK_DBG 0);
        glUniformMatrix4fv(m_matrix, 1, GL_FALSE, (const GLfloat*) &m_m[0][0]); __testGL(BLIK_DBG 0);
    }

    void NativePainter::initShaderES20()
    {
        initShaderES(
            "attribute highp vec2 a_position;\n"
            "attribute highp vec4 a_color;\n"
            "uniform highp mat4 u_matrix;\n"
            "varying mediump vec4 v_fragmentColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_matrix * vec4(a_position.x, a_position.y, 0.0, 1.0);\n"
            "    v_fragmentColor = a_color;\n"
            "}",
            "uniform highp mat4 u_matrix;\n"
            "varying mediump vec4 v_fragmentColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_FragColor = v_fragmentColor;\n"
            "}");
    }

    void NativePainter::initShaderES30()
    {
        initShaderES(String::Format(
            "#version 300 es\n"
            "layout (location = %d) in highp vec2 a_position;\n"
            "layout (location = %d) in highp vec4 a_color;\n"
            "uniform highp mat4 u_matrix;\n"
            "out mediump vec4 v_fragmentColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_matrix * vec4(a_position.x, a_position.y, 0.0, 1.0);\n"
            "    v_fragmentColor = a_color;\n"
            "}", VerticeID, ColorID),
            "#version 300 es\n"
            "layout (location = 0) out highp vec4 oColour;\n"
            "uniform highp mat4 u_matrix;\n"
            "in mediump vec4 v_fragmentColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    oColour = v_fragmentColor;\n"
            "}");
    }

    void NativePainter::termShaderES()
    {
        glDeleteProgram(m_program); __testGL(BLIK_DBG 0);
        m_program = 0;
        glDeleteShader(m_vshader); __testGL(BLIK_DBG 0);
        m_vshader = 0;
        glDeleteShader(m_fshader); __testGL(BLIK_DBG 0);
        m_fshader = 0;
    }

    void NativePainter::__testGL(BLIK_DBG_PRM sint32 nouse)
    {
        if(auto errorCode = glGetError())
            BLIK_ASSERT_PRM(String::Format("__testGL(error:%d) is failed", errorCode), false);
    }

    uint08 NativePainter::__version()
    {
        static uint08 Version = 0x00;
        if(Version != 0x00) return Version;

        chars VersionString = (chars) glGetString(GL_VERSION);
        // 예시1: OpenGL ES 2.0 IMGSGX543-124.1
        // 예시2: OpenGL ES 3.0 APPLE-12.0.38
        BLIK_ASSERT("이해할 수 없는 버전정보입니다",
            VersionString[9] == ' ' && VersionString[11] == '.' && VersionString[13] == ' ');

        const uint32 VersionMajor = VersionString[10] - '0';
        const uint32 VersionMinor = VersionString[12] - '0';
        Version = ((VersionMajor & 0xF) << 4) | (VersionMinor & 0xF);
        return Version;
    }

    void NativePainter::__testShader(BLIK_DBG_PRM GLuint shader)
    {
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(status == GL_FALSE)
        {
            GLchar log[4096];
            GLsizei s;
            glGetShaderInfoLog(shader, 4096, &s, log);
            BLIK_ASSERT_PRM(String::Format("__testShader(%s) is failed", log), false);
        }
        else if(auto errorCode = glGetError())
            BLIK_ASSERT_PRM(String::Format("__testShader(error:%d) is failed", errorCode), false);
    }

    void NativePainter::__testProgram(BLIK_DBG_PRM GLuint program)
    {
        GLint linked;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if(!linked)
        {
            int i32InfoLogLength, i32CharsWritten;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
            char* pszInfoLog = new char[i32InfoLogLength];
            glGetProgramInfoLog(program, i32InfoLogLength, &i32CharsWritten, pszInfoLog);
            BLIK_ASSERT_PRM(String::Format("__testProgram(%s) is failed", pszInfoLog), false);
            delete [] pszInfoLog;
        }
        else if(auto errorCode = glGetError())
            BLIK_ASSERT_PRM(String::Format("__testProgram(error:%d) is failed", errorCode), false);
    }

    void NativePainter::__loadIdentity()
    {
        m_m[0][0] = 1; m_m[0][1] = 0; m_m[0][2] = 0; m_m[0][3] = 0;
        m_m[1][0] = 0; m_m[1][1] = 1; m_m[1][2] = 0; m_m[1][3] = 0;
        m_m[2][0] = 0; m_m[2][1] = 0; m_m[2][2] = 1; m_m[2][3] = 0;
        m_m[3][0] = 0; m_m[3][1] = 0; m_m[3][2] = 0; m_m[3][3] = 1;
    }

    void NativePainter::__multiply(
        const float m00, const float m01, const float m02, const float m03,
        const float m10, const float m11, const float m12, const float m13,
        const float m20, const float m21, const float m22, const float m23,
        const float m30, const float m31, const float m32, const float m33)
    {
        GLfloat m[4][4];
        Memory::Copy(&m[0][0], &m_m[0][0], sizeof(GLfloat) * 16);
        m_m[0][0] = m[0][0] * m00 + m[0][1] * m10 + m[0][2] * m20 + m[0][3] * m30;
        m_m[0][1] = m[0][0] * m01 + m[0][1] * m11 + m[0][2] * m21 + m[0][3] * m31;
        m_m[0][2] = m[0][0] * m02 + m[0][1] * m12 + m[0][2] * m22 + m[0][3] * m32;
        m_m[0][3] = m[0][0] * m03 + m[0][1] * m13 + m[0][2] * m23 + m[0][3] * m33;
        m_m[1][0] = m[1][0] * m00 + m[1][1] * m10 + m[1][2] * m20 + m[1][3] * m30;
        m_m[1][1] = m[1][0] * m01 + m[1][1] * m11 + m[1][2] * m21 + m[1][3] * m31;
        m_m[1][2] = m[1][0] * m02 + m[1][1] * m12 + m[1][2] * m22 + m[1][3] * m32;
        m_m[1][3] = m[1][0] * m03 + m[1][1] * m13 + m[1][2] * m23 + m[1][3] * m33;
        m_m[2][0] = m[2][0] * m00 + m[2][1] * m10 + m[2][2] * m20 + m[2][3] * m30;
        m_m[2][1] = m[2][0] * m01 + m[2][1] * m11 + m[2][2] * m21 + m[2][3] * m31;
        m_m[2][2] = m[2][0] * m02 + m[2][1] * m12 + m[2][2] * m22 + m[2][3] * m32;
        m_m[2][3] = m[2][0] * m03 + m[2][1] * m13 + m[2][2] * m23 + m[2][3] * m33;
        m_m[3][0] = m[3][0] * m00 + m[3][1] * m10 + m[3][2] * m20 + m[3][3] * m30;
        m_m[3][1] = m[3][0] * m01 + m[3][1] * m11 + m[3][2] * m21 + m[3][3] * m31;
        m_m[3][2] = m[3][0] * m02 + m[3][1] * m12 + m[3][2] * m22 + m[3][3] * m32;
        m_m[3][3] = m[3][0] * m03 + m[3][1] * m13 + m[3][2] * m23 + m[3][3] * m33;
    }

    ViewAPI::ViewAPI(View* manager)
    {
        m_view_manager = manager;
        m_next_manager = nullptr;
    }

    ViewAPI::ViewAPI(const ViewAPI& rhs)
    {BLIK_ASSERT("호출불가", false);}

    ViewAPI& ViewAPI::operator=(const ViewAPI& rhs)
    {BLIK_ASSERT("호출불가", false); return *this;}

    ViewAPI::~ViewAPI()
    {
        delete m_view_manager;
        delete m_next_manager;
    }

    NativePainter*& ViewAPI::CurPainter()
    {static NativePainter* _ = nullptr; return _;}

    BLIK::Color& ViewAPI::CurColor()
    {static BLIK::Color _; return _;}

    h_view ViewAPI::changeViewManager(View* manager)
    {
        h_view OldViewHandle;
        if(m_view_manager)
        {
            OldViewHandle = m_view_manager->SetView(h_view::null());
            delete m_view_manager;
        }

        m_view_manager = manager;
        return OldViewHandle;
    }

    void ViewAPI::setNextViewManager(View* manager)
    {
        delete m_next_manager;
        m_next_manager = manager;
    }

    void ViewAPI::render(sint32 width, sint32 height)
    {
        static NativePainter Painter;
        // for assert dialog
        if(CurPainter()) return;

        Painter.reset(width, height);
        CurPainter() = &Painter;
        m_view_manager->OnRender(width, height, 0, 0, width, height);
        CurPainter() = nullptr;

        if(m_next_manager)
        {
            h_view OldViewHandle = changeViewManager(m_next_manager);
            m_next_manager->SetView(OldViewHandle);
            m_next_manager = nullptr;
            sendCreate();
        }
    }

    void ViewAPI::touch(TouchType type, sint32 id, sint32 x, sint32 y)
    {m_view_manager->OnTouch(type, id, x, y);}

    void ViewAPI::sendCreate()
    {m_view_manager->OnCreate();}

#endif
