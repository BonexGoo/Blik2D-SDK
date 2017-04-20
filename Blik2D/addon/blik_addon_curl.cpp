#include <blik.h>

#if !defined(BLIK_NEED_ADDON_CURL) || (BLIK_NEED_ADDON_CURL != 0 && BLIK_NEED_ADDON_CURL != 1)
    #error BLIK_NEED_ADDON_CURL macro is invalid use
#endif
bool __LINK_ADDON_CURL__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BLIK_NEED_ADDON_CURL

#include "blik_addon_curl.hpp"

#include <addon/blik_addon.hpp>
#include <platform/blik_platform.hpp>

extern "C"
{
    typedef void (RTMP_LogCallback)(int level, const char *fmt, va_list);
    void RTMP_LogSetCallback(RTMP_LogCallback *cb);
}

// 등록과정
namespace BLIK
{
    BLIK_DECLARE_ADDON_FUNCTION(Curl, Create, id_curl, void)
    BLIK_DECLARE_ADDON_FUNCTION(Curl, Clone, id_curl, id_curl)
    BLIK_DECLARE_ADDON_FUNCTION(Curl, Release, void, id_curl)
    BLIK_DECLARE_ADDON_FUNCTION(Curl, RequestString, chars, id_curl, chars, chars, chars)
    BLIK_DECLARE_ADDON_FUNCTION(Curl, RequestBytes, bytes, id_curl, chars, sint32*, chars, chars)
    BLIK_DECLARE_ADDON_FUNCTION(Curl, RequestRedirectUrl, chars, id_curl, chars, sint32, chars, chars)
    BLIK_DECLARE_ADDON_FUNCTION(Curl, SendStream, void, id_curl, chars, CurlReadCB, payload)

    static autorun Bind_AddOn_Curl()
    {
        Core_AddOn_Curl_Create() = Customized_AddOn_Curl_Create;
        Core_AddOn_Curl_Clone() = Customized_AddOn_Curl_Clone;
        Core_AddOn_Curl_Release() = Customized_AddOn_Curl_Release;
        Core_AddOn_Curl_RequestString() = Customized_AddOn_Curl_RequestString;
        Core_AddOn_Curl_RequestBytes() = Customized_AddOn_Curl_RequestBytes;
        Core_AddOn_Curl_RequestRedirectUrl() = Customized_AddOn_Curl_RequestRedirectUrl;
        Core_AddOn_Curl_SendStream() = Customized_AddOn_Curl_SendStream;

        // 로그연결
        #if !BLIK_NDEBUG
            RTMP_LogSetCallback(
                [](int level, const char* fmt, va_list args)->void
                {
                    char Temp[1024];
                    blik_vsnprintf(Temp, 1024, fmt, args);
                    BLIK_TRACE("[RTMP LV-%d] %s", level, Temp);
                });
        #endif
        return true;
    }
    static autorun _ = Bind_AddOn_Curl();
}

static size_t CurlWriteToResult(char* ptr, size_t size, size_t nitems, void* outstream)
{
    size_t realsize = size * nitems;
    *((String*) outstream) += String((chars) ptr, realsize);
    return realsize;
}

static size_t CurlReadFromAsset(void* ptr, size_t size, size_t nitems, void* stream)
{
    size_t realsize = Platform::File::Read((id_file_read) stream, (uint08*) ptr, size * nitems);
    BLIK_TRACE("CurlReadFromAsset: %d bytes", (sint32) realsize);
    return realsize;
}

static size_t CurlWriteForAssert(char* ptr, size_t size, size_t nitems, void* outstream)
{
    size_t realsize = size * nitems;
    BLIK_ASSERT((chars) ptr, false);
    return realsize;
}

static size_t CurlWriteToUint08s(char* ptr, size_t size, size_t nitems, void* outstream)
{
    size_t realsize = size * nitems;
    uint08s& Result = *((uint08s*) outstream);
    Memory::Copy(Result.AtDumpingAdded(realsize), ptr, realsize);
    return realsize;
}

// 구현부
namespace BLIK
{
    struct CurlStruct
    {
        CURL* mId;
        sint32 mRefCount;
    };

    id_curl Customized_AddOn_Curl_Create(void)
    {
        CurlStruct* NewStruct = new CurlStruct();
        NewStruct->mId = curl_easy_init();
        NewStruct->mRefCount = 1;
        return (id_curl) NewStruct;
    }

    id_curl Customized_AddOn_Curl_Clone(id_curl curl)
    {
        if(!curl) return nullptr;
        CurlStruct* CurStruct = (CurlStruct*) curl;
        CurStruct->mRefCount++;
        return (id_curl) CurStruct;
    }

    void Customized_AddOn_Curl_Release(id_curl curl)
    {
        if(!curl) return;
        CurlStruct* OldStruct = (CurlStruct*) curl;
        if(--OldStruct->mRefCount == 0)
        {
            curl_easy_cleanup(OldStruct->mId);
            delete OldStruct;
        }
    }

    static uint08s _RequestCore(id_curl curl, chars url, chars postdata, chars headerdata, String* redirect_url, sint32 successcode)
    {
        static uint08s Result;
        Result.SubtractionAll();
        if(!curl) return Result;
        CURL* CurCurl = ((CurlStruct*) curl)->mId;

        curl_easy_setopt(CurCurl, CURLOPT_URL, url);
        curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
        if(postdata)
        {
            curl_easy_setopt(CurCurl, CURLOPT_POST, 1);
            curl_easy_setopt(CurCurl, CURLOPT_POSTFIELDS , postdata);
            curl_easy_setopt(CurCurl, CURLOPT_POSTFIELDSIZE, blik_strlen(postdata));
        }
        else curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, &Result);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, CurlWriteToUint08s);

        String Host;
        for(chars iurl = url + 2, ibegin = nullptr; true; ++iurl)
        {
            if(*iurl == '\0')
            {
                if(ibegin)
                    Host = String(ibegin, iurl - ibegin);
                break;
            }
            else if(*iurl == '/')
            {
                if(ibegin)
                {
                    Host = String(ibegin, iurl - ibegin);
                    break;
                }
                else if(iurl[-2] == ':' && iurl[-1] == '/')
                    ibegin = iurl + 1;
            }
        }
        String Referer;
        for(chars iurl = url; true; ++iurl)
        {
            if(*iurl == '\0' || *iurl == '?')
            {
                Referer = String(url, iurl - url);
                break;
            }
        }

        curl_slist* cheader = nullptr;
        cheader = curl_slist_append(cheader, "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.0.3705)");
        cheader = curl_slist_append(cheader, String::Format("Host: %s", (chars) Host));
        cheader = curl_slist_append(cheader, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
        cheader = curl_slist_append(cheader, "Accept-Language: ko-kr,ko;q=0.8,en-us;q=0.5,en;q=0.3");
        cheader = curl_slist_append(cheader, "Connection: keep-alive");
        cheader = curl_slist_append(cheader, String::Format("Referer: %s", (chars) Referer));
        cheader = curl_slist_append(cheader, "Content-Type: application/x-www-form-urlencoded");
        if(headerdata) cheader = curl_slist_append(cheader, headerdata);
        curl_easy_setopt(CurCurl, CURLOPT_HTTPHEADER, cheader);

        CURLcode res = curl_easy_perform(CurCurl);
        curl_slist_free_all(cheader);

        if(res == CURLE_OK)
        {
            long statLong = 0;
            if(CURLE_OK == curl_easy_getinfo(CurCurl, CURLINFO_HTTP_CODE, &statLong))
            if(redirect_url && statLong == successcode)
            {
                char* location = nullptr;
                curl_easy_getinfo(CurCurl, CURLINFO_REDIRECT_URL, &location);
                *redirect_url = location;
            }
        }
        return Result;
    }

    chars Customized_AddOn_Curl_RequestString(id_curl curl, chars url, chars postdata, chars headerdata)
    {
        static String Result;
        uint08s RequestResult = _RequestCore(curl, url, postdata, headerdata, nullptr, 0);
        if(RequestResult.Count() == 0) return "";

        Result = String((chars) RequestResult.AtDumping(0, 1), RequestResult.Count());
        return Result;
    }

    bytes Customized_AddOn_Curl_RequestBytes(id_curl curl, chars url, sint32* getsize, chars postdata, chars headerdata)
    {
        static uint08s Result;
        Result = _RequestCore(curl, url, postdata, headerdata, nullptr, 0);
        if(getsize) *getsize = Result.Count();
        return Result.AtDumping(0, 1);
    }

    chars Customized_AddOn_Curl_RequestRedirectUrl(id_curl curl, chars url, sint32 successcode, chars postdata, chars headerdata)
    {
        static String Result;
        Result = "";
        _RequestCore(curl, url, postdata, headerdata, &Result, successcode);
        return Result;
    }

    void Customized_AddOn_Curl_SendStream(id_curl curl, chars url, CurlReadCB cb, payload data)
    {
        if(!curl) return;
        CURL* CurCurl = ((CurlStruct*) curl)->mId;
        curl_easy_setopt(CurCurl, CURLOPT_URL, url);
        curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
        curl_easy_setopt(CurCurl, CURLOPT_UPLOAD, 1);
        curl_easy_setopt(CurCurl, CURLOPT_READDATA, data);
        curl_easy_setopt(CurCurl, CURLOPT_READFUNCTION, cb);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, nullptr);
        curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, CurlWriteForAssert);
        curl_easy_setopt(CurCurl, CURLOPT_VERBOSE, 1);
        curl_easy_perform(CurCurl);
    }
}

#endif
