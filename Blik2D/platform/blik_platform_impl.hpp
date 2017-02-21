#pragma once
#include <platform/blik_platform.hpp>

namespace BLIK
{
    namespace PlatformImpl
    {
        // 일반함수와 전역인스턴스
        namespace Core
        {
            chars NormalPath(chars itemname, bool QCodeTest = true);
            wchars NormalPathW(wchars itemname, bool QCodeTest = true);
            sint32 CreateManager(buffer manager);
            void RemoveManager(sint32 id);
            buffer GetManager(sint32 id);
            template<typename TYPE, datatype DATATYPE>
            TYPE* GetManagerBy(sint32 id)
            {
                buffer Result = GetManager(id);
                const bool IsResultEnabled = Buffer::TypeCheck<TYPE, DATATYPE>(Result);
                BLIK_ASSERT("Manager의 Type이 일치하지 않습니다", IsResultEnabled);
                return (TYPE*) Result;
            }

            extern String g_AssetsRoot;
            extern String g_AssetsRemRoot;
        }

        // 래핑함수
        namespace Wrap
        {
            sint64 Utility_CurrentAvailableMemory(sint64* totalbytes);
			void Utility_SetOptionFlag(chars name, bool flag);
			bool Utility_GetOptionFlag(chars name);
			Strings Utility_GetOptionFlagNames();
			void Utility_SetOptionPayload(chars name, payload data);
			payload Utility_GetOptionPayload(chars name);
			Strings Utility_GetOptionPayloadNames();
            bool Popup_FileDialog(String& path, String* shortpath, chars title, bool isdir);
			void Popup_WebBrowserDialog(String url);
            WString File_GetDirName(wchars itemname, wchar_t badslash, wchar_t goodslash);
            WString File_GetShortName(wchars itemname);
            sint32 File_GetDriveCode();
            bool File_Tempname(char* format, sint32 length);
            void* File_FDMap(sint32 fd, sint64 offset, sint64 size, bool readonly);
            bool File_FDUnmap(const void* map);
            void File_ResetAssetsRoot(chars dirname);
            void File_ResetAssetsRemRoot(chars dirname);
        }
    }
}
