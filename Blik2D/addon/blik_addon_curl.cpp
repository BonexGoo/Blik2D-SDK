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
    BLIK_DECLARE_ADDON_FUNCTION(Curl, ServiceRequest, chars, id_curl, chars, chars)
    BLIK_DECLARE_ADDON_FUNCTION(Curl, SendStream, void, id_curl, chars, chars, CurlReadCB, payload)

    static autorun Bind_AddOn_Curl()
    {
        Core_AddOn_Curl_Create() = Customized_AddOn_Curl_Create;
        Core_AddOn_Curl_Clone() = Customized_AddOn_Curl_Clone;
        Core_AddOn_Curl_Release() = Customized_AddOn_Curl_Release;
        Core_AddOn_Curl_RequestString() = Customized_AddOn_Curl_RequestString;
        Core_AddOn_Curl_RequestBytes() = Customized_AddOn_Curl_RequestBytes;
        Core_AddOn_Curl_RequestRedirectUrl() = Customized_AddOn_Curl_RequestRedirectUrl;
        Core_AddOn_Curl_ServiceRequest() = Customized_AddOn_Curl_ServiceRequest;
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

    chars Customized_AddOn_Curl_ServiceRequest(id_curl curl, chars service, chars arg)
    {
        if(!curl) return "";
        CURL* CurCurl = ((CurlStruct*) curl)->mId;

        const String ClientId = "575714888985-k3vms25oep7ev6mo0k749aaildj5ltjf.apps.googleusercontent.com";
        const String ClientSecret = "uBPaOngNjODdI9xMC1QuIwiX";
        static String Result;
        Result = "";

        // 구글+ 인증코드 받기
        if(!String::Compare(service, "google+ OAuth-A"))
        {
            const String ProjectId = "crypto-arcade-149707";
            const String AuthUri = "https://accounts.google.com/o/oauth2/auth";
            const String TokenUri = "https://accounts.google.com/o/oauth2/token";
            const String AuthProviderX509CertUrl = "https://www.googleapis.com/oauth2/v1/certs";

            const String Url = String::Format("https://accounts.google.com/o/oauth2/auth?"
                "client_id=%s&"
                "redirect_uri=urn:ietf:wg:oauth:2.0:oob&"
                "scope=https://www.googleapis.com/auth/youtube&"
                "response_type=code&"
                "access_type=offline", (chars) ClientId);
            curl_easy_setopt(CurCurl, CURLOPT_URL, (chars) Url);
            curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
            curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, &Result);
            curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, CurlWriteToResult);

            curl_slist* cheader = nullptr;
            cheader = curl_slist_append(cheader, "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.0.3705)");
            cheader = curl_slist_append(cheader, "Host: accounts.google.com");
            cheader = curl_slist_append(cheader, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
            cheader = curl_slist_append(cheader, "Accept-Language: en-US,en;q=0.5");
            cheader = curl_slist_append(cheader, "Connection: keep-alive");
            cheader = curl_slist_append(cheader, "Referer: https://accounts.google.com/o/oauth2/auth");
            cheader = curl_slist_append(cheader, "Content-Type: application/x-www-form-urlencoded");
            curl_easy_setopt(CurCurl, CURLOPT_HTTPHEADER, cheader);

            CURLcode res = curl_easy_perform(CurCurl);
            curl_slist_free_all(cheader);

            if(res == CURLE_OK)
            {
                long statLong = 0;
                if(CURLE_OK == curl_easy_getinfo(CurCurl, CURLINFO_HTTP_CODE, &statLong) && statLong == 302)
                {
                    char* location;
                    curl_easy_getinfo(CurCurl, CURLINFO_REDIRECT_URL, &location);
                    Result = location;
                    return Result;
                }
            }
        }
        // 구글+ 액세스토큰/새로고침토큰 받기
        else if(!String::Compare(service, "google+ OAuth-B"))
        {
            curl_easy_setopt(CurCurl, CURLOPT_URL, "https://accounts.google.com/o/oauth2/token");
            curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0);
            String PostData = String::Format(
                "code=%s&"
                "client_id=%s&"
                "client_secret=%s&"
                "redirect_uri=urn:ietf:wg:oauth:2.0:oob&"
                "grant_type=authorization_code", arg, (chars) ClientId, (chars) ClientSecret);
            curl_easy_setopt(CurCurl, CURLOPT_POST, 1);
            curl_easy_setopt(CurCurl, CURLOPT_POSTFIELDS , (chars) PostData);
            curl_easy_setopt(CurCurl, CURLOPT_POSTFIELDSIZE, PostData.Length());
            curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, &Result);
            curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, CurlWriteToResult);

            curl_slist* cheader = nullptr;
            cheader = curl_slist_append(cheader, "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.0.3705)");
            cheader = curl_slist_append(cheader, "Host: accounts.google.com");
            cheader = curl_slist_append(cheader, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
            cheader = curl_slist_append(cheader, "Accept-Language: en-US,en;q=0.5");
            cheader = curl_slist_append(cheader, "Connection: keep-alive");
            cheader = curl_slist_append(cheader, "Referer: https://accounts.google.com/o/oauth2/token");
            cheader = curl_slist_append(cheader, "Content-Type: application/x-www-form-urlencoded");
            curl_easy_setopt(CurCurl, CURLOPT_HTTPHEADER, cheader);

            CURLcode res = curl_easy_perform(CurCurl);
            curl_slist_free_all(cheader);

            if(res == CURLE_OK)
                return Result;
        }
        // 유투브 DataAPI 호출
        else if(!String::Compare(service, "youtube"))
        {
            const String Url = String::Format("https://www.googleapis.com/youtube/v3/channels?"
                "part=id&"
                "mine=true&"
                "access_token=%s", arg);
            curl_easy_setopt(CurCurl, CURLOPT_URL, (chars) Url);
            curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
            curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, &Result);
            curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, CurlWriteToResult);

            curl_slist* cheader = nullptr;
            cheader = curl_slist_append(cheader, "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.0.3705)");
            cheader = curl_slist_append(cheader, "Host: www.googleapis.com");
            cheader = curl_slist_append(cheader, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
            cheader = curl_slist_append(cheader, "Accept-Language: en-US,en;q=0.5");
            cheader = curl_slist_append(cheader, "Connection: keep-alive");
            cheader = curl_slist_append(cheader, "Referer: https://www.googleapis.com/youtube/v3/channels");
            cheader = curl_slist_append(cheader, "Content-Type: application/x-www-form-urlencoded");
            curl_easy_setopt(CurCurl, CURLOPT_HTTPHEADER, cheader);

            CURLcode res = curl_easy_perform(CurCurl);
            curl_slist_free_all(cheader);

            if(res == CURLE_OK)
                return Result;
        }
        // 유투브 LiveStreamAPI 호출
        else if(!String::Compare(service, "youtube LiveStreams:list"))
        {
            const String Url = String::Format("https://www.googleapis.com/youtube/v3/liveStreams?"
                "part=id,snippet,cdn,status&"
                "mine=true&"
                "access_token=%s", arg);
            curl_easy_setopt(CurCurl, CURLOPT_URL, (chars) Url);
            curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
            curl_easy_setopt(CurCurl, CURLOPT_WRITEDATA, &Result);
            curl_easy_setopt(CurCurl, CURLOPT_WRITEFUNCTION, CurlWriteToResult);

            curl_slist* cheader = nullptr;
            cheader = curl_slist_append(cheader, "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.0.3705)");
            cheader = curl_slist_append(cheader, "Host: www.googleapis.com");
            cheader = curl_slist_append(cheader, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
            cheader = curl_slist_append(cheader, "Accept-Language: en-US,en;q=0.5");
            cheader = curl_slist_append(cheader, "Connection: keep-alive");
            cheader = curl_slist_append(cheader, "Referer: https://www.googleapis.com/youtube/v3/liveStreams");
            cheader = curl_slist_append(cheader, "Content-Type: application/x-www-form-urlencoded");
            curl_easy_setopt(CurCurl, CURLOPT_HTTPHEADER, cheader);

            CURLcode res = curl_easy_perform(CurCurl);
            curl_slist_free_all(cheader);

            if(res == CURLE_OK)
                return Result;
        }
        // 유투브 RTMP 업로드
        else if(!String::Compare(service, "youtube rtmp"))
        {
            id_file_read OneFile = Platform::File::OpenForRead(Platform::File::RootForAssets() + "video/test2.flv");
            const String Url = String::Format("rtmp://a.rtmp.youtube.com/live2/%s", "w6gp-fg13-cyew-a5gp");
            curl_easy_setopt(CurCurl, CURLOPT_URL, (chars) Url);
            curl_easy_setopt(CurCurl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(CurCurl, CURLOPT_POST, 0);
            curl_easy_setopt(CurCurl, CURLOPT_UPLOAD, 1);
            curl_easy_setopt(CurCurl, CURLOPT_READDATA, OneFile);
            curl_easy_setopt(CurCurl, CURLOPT_READFUNCTION, CurlReadFromAsset);
            curl_easy_setopt(CurCurl, CURLOPT_VERBOSE, 1);

            /*/////////////////////////////////////////////////////////////////////////////////////
            곰스튜디오는 w6gp-fg13-cyew-a5gp대신 dh0d-y11p-3kre-4cky를 사용한다
            <곰스튜디오에 더 있는 내용>
            ..flashVer [00 08]
            ...FMLE/3.0 (compatible; FMSc/1.0) [02 00 1F]
            ..swfUrl [00 06]
            ...rtmp://a.rtmp.youtube.com/live2 [02 00 1F]
            /////////////////////////////////////////////////////////////////////////////////////*/
            //스트림이름: php7-efmg-4r16-fae0
            //기본서버URL: rtmp://a.rtmp.youtube.com/live2
            //백업서버URL: rtmp://b.rtmp.youtube.com/live2?backup=1
            /////////////////////////////////////////////////////////////////////////////////////
            /*프로토콜:    RTMP 플래시 스트리밍
            동영상 코덱:    H.264, 4.1
            프레임 속도:    최대 60fps
            키프레임 빈도:    
            2초 권장
            4초를 초과하지 말 것
            오디오 코덱:    AAC 또는 MP3
            전송률 인코딩:    CBR*/

            CURLcode res = curl_easy_perform(CurCurl);
            Platform::File::Close(OneFile);

            if(res == CURLE_OK)
                return Result;
        }
        return "";

        //유투브: "AIzaSyDuz6-YUh6v4R1WTqNzm5eDAOxirHfsnH0"
        //client_id: "575714888985-k3vms25oep7ev6mo0k749aaildj5ltjf.apps.googleusercontent.com"
        //client_secret: "uBPaOngNjODdI9xMC1QuIwiX"
        //스트림서버주소: "rtmp://a.rtmp.youtube.com/live2"
        //스트림이름및키: "w6gp-fg13-cyew-a5gp"

        // 구글API 도움말사이트: https://developers.google.com/youtube/v3/guides/auth/server-side-web-apps?hl=ko
        // curl 도움말사이트-1: http://devkyu.tistory.com/43
        // curl 도움말사이트-2: http://xshine.tistory.com/251

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /*curl_easy_setopt(curl, CURLOPT_URL, "https://accounts.google.com/o/oauth2/token");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        String OAuth2PostData = String::Format(
            "code=4/ux5gNj-_mIu4DOD_gNZdjX9EtOFf&"
            "client_id=%s&"
            "client_secret=%s&"
            "redirect_uri=%s&"
            "grant_type=refresh_token", (chars) ClientId, (chars) ClientSecret, (chars) RedirectUris[1]);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS , (chars) OAuth2PostData);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0);*/

        /*
        // OAuth2 자격질의
        String OAuth2Request = String::Format("{ installed : {"
            "\"client_id\": \"%s\","
            "\"client_secret\": \"%s\","
            "\"redirect_uri\" : \"urn:ietf:wg:oauth:2.0:oob\" } }", (chars) ClientID, (chars) ClientSecret);
        */

        /*curl_easy_setopt(curl, CURLOPT_URL, "https://www.googleapis.com/youtube/v3/liveStreams?"
            "scope=https://www.googleapis.com/auth/youtube"
            "&part=id, snippet"
            "&id=w6gp-fg13-cyew-a5gp"
            "&mine=true");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);*/

        // OAuth2 호출
        //https://accounts.google.com/b/0/DelegateAccountSelector?continue=https://accounts.google.com/o/oauth2/auth?client_id%3D292824132082.apps.googleusercontent.com%26scope%3Dhttps://www.googleapis.com/auth/youtube%26immediate%3Dfalse%26include_granted_scopes%3Dfalse%26proxy%3Doauth2relay495956414%26redirect_uri%3Dpostmessage%26origin%3Dhttps://explorer.apis.google.com%26response_type%3Dtoken%26gsiwebsdk%3D1%26state%3D436712854%257C0.3932729874%26authuser%3D0%26jsh%3Dm;/_/scs/apps-static/_/js/k%253Doz.gapi.ko.pkIpgFn_TBQ.O/m%253D__features__/am%253DAQ/rt%253Dj/d%253D1/rs%253DAGLTcCPMtqW3Zv-8f7O5mnsp9fyRCEM7kg
        // 클라ID변경: https://accounts.google.com/b/0/DelegateAccountSelector?continue=https://accounts.google.com/o/oauth2/auth?client_id%3D575714888985-k3vms25oep7ev6mo0k749aaildj5ltjf.apps.googleusercontent.com%26scope%3Dhttps://www.googleapis.com/auth/youtube%26immediate%3Dfalse%26include_granted_scopes%3Dfalse%26proxy%3Doauth2relay495956414%26redirect_uri%3Dhttp://localhost%26origin%3Dhttps://explorer.apis.google.com%26response_type%3Dtoken%26gsiwebsdk%3D1%26state%3D436712854%257C0.3932729874%26authuser%3D0%26jsh%3Dm;/_/scs/apps-static/_/js/k%253Doz.gapi.ko.pkIpgFn_TBQ.O/m%253D__features__/am%253DAQ/rt%253Dj/d%253D1/rs%253DAGLTcCPMtqW3Zv-8f7O5mnsp9fyRCEM7kg
            
        /*"https://accounts.google.com/b/0/DelegateAccountSelector?"
        "continue=https://accounts.google.com/o/oauth2/auth?client_id=292824132082.apps.googleusercontent.com"
        "&scope=https://www.googleapis.com/auth/youtube&immediate=false&include_granted_scopes=false&proxy=oauth2relay495956414"
        "&redirect_uri=postmessage"
        "&origin=https://explorer.apis.google.com"
        "&response_type=token"
        "&gsiwebsdk=1"
        "&state=436712854|0.3932729874"
        "&authuser=0"
        "&jsh=m;/_/scs/apps-static/_/js/k=oz.gapi.ko.pkIpgFn_TBQ.O/m=__features__/am=AQ/rt=j/d=1/rs=AGLTcCPMtqW3Zv-8f7O5mnsp9fyRCEM7kg"*/

        //html_context_data data = {0, 0};
        //curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_write_memory);

        /*curl_easy_setopt(curl, CURLOPT_URL, "https://www.googleapis.com/youtube/v3/liveStreams");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);*/

        /*쿠키얻기
        struct curl_slist *cookie_list, *t = NULL;
        curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookie_list);
        t = cookie_list;
        while (t) {
            BLIK_TRACE(t->data);
            t = t->next;
        }
        curl_slist_free_all(cookie_list);*/

        /*CURLcode res = curl_easy_perform(curl);
        if(res == CURLE_OK)
        {
            double statDouble ;
            long statLong ;
            char* statString = NULL ;

            // HTTP 응답코드를 얻어온다. 
            if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_HTTP_CODE , &statLong ) )
                BLIK_TRACE("Response code:  %d", (int) statLong);

            // Content-Type 를 얻어온다.
            if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_CONTENT_TYPE , &statString ) )
                BLIK_TRACE("Content type:   %s", statString);

            // 다운로드한 문서의 크기를 얻어온다.
            if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_SIZE_DOWNLOAD , &statDouble ) )
                BLIK_TRACE("Download size:  %d bytes", (int) statDouble);

            if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_SPEED_DOWNLOAD , &statDouble ) )
                BLIK_TRACE("Download speed: %d bytes/sec", (int) statDouble);
        }*/

        //curl_slist_free_all(cheader);
        //curl_easy_cleanup(curl);
    }

    void Customized_AddOn_Curl_SendStream(id_curl curl, chars service, chars key, CurlReadCB cb, payload data)
    {
        if(!curl) return;
        CURL* CurCurl = ((CurlStruct*) curl)->mId;

        // 구글
        if(!String::Compare(service, "YouTube"))
        {
            const String Url = String::Format("rtmp://a.rtmp.youtube.com/live2/%s", key);
            curl_easy_setopt(CurCurl, CURLOPT_URL, (chars) Url);
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
        // 구글
        else if(!String::Compare(service, "Facebook"))
        {
            const String Url = String::Format("rtmp://rtmp-api.facebook.com:80/rtmp/%s", key);
            curl_easy_setopt(CurCurl, CURLOPT_URL, (chars) Url);
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
}

#endif
