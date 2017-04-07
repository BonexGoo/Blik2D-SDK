#include <blik.hpp>
#include "blik_oauth2service.hpp"

#include <addon/blik_addon.hpp>
#include <element/blik_image.hpp>
#include <format/blik_bmp.hpp>

namespace BLIK
{
    ////////////////////////////////////////////////////////////////////////////////
    // Google+
    ////////////////////////////////////////////////////////////////////////////////
    class OAuth2GoogleService : public OAuth2Service
    {
    private:
        class Data
        {
        public:
            Data()
            {
                mNeedDestroyWeb = false;
                mCurl = AddOn::Curl::Create();
                mPictureClipper = nullptr;
            }
            ~Data()
            {
                Platform::Web::Release(mWeb);
                AddOn::Curl::Release(mCurl);
                Bmp::Remove(mPictureClipper);
            }
            Data(const Data& rhs) {operator=(rhs);}
            Data& operator=(const Data& rhs)
            {BLIK_ASSERT("This class is not allowed to be copied", false); return *this;}
        public:
            h_web mWeb;
            bool mNeedDestroyWeb;
            id_curl mCurl;
            String mClientId;
            String mClientSecret;
            String mAccessToken;
            String mRefreshToken;
            String mName;
            String mComment;
            Image mPicture;
            Image mBackground;
            id_bitmap mPictureClipper;
        };

    public:
        OAuth2GoogleService()
        {
            mShare = Share::Create(Buffer::Alloc<Data>(BLIK_DBG 1));
        }
        ~OAuth2GoogleService() override
        {
            Share::Remove(mShare);
        }

    public:
        void Signin(chars option, id_bitmap clipper) override
        {
            Context ClientInfo(ST_Json, SO_OnlyReference, option);
            data().mClientId = ClientInfo[0].GetString();
            data().mClientSecret = ClientInfo[1].GetString();
            Bmp::Remove(data().mPictureClipper);
            data().mPictureClipper = clipper;

            const String QueryUrl = String::Format(
                "https://" "accounts.google.com/o/oauth2/auth?"
                "client_id=%s&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "scope=https://" "www.googleapis.com/auth/plus.me&"
                "response_type=code&"
                "access_type=offline", (chars) data_const().mClientId);
            String ResultUrl = AddOn::Curl::RequestRedirectUrl(data().mCurl, QueryUrl, 302);
            data().mWeb = Platform::Web::Create(ResultUrl, 0, 0, OnEvent, (payload) this);
            data().mNeedDestroyWeb = false;
        }
        bool IsSigning() override
        {
            if(data().mNeedDestroyWeb)
            {
                data().mNeedDestroyWeb = false;
                Platform::Web::Release(data().mWeb);
                return false;
            }
            return (data_const().mWeb.get() != nullptr);
        }
        id_image_read GetWebImage(sint32 width, sint32 height) override
        {
            if(IsSigning())
            {
                Platform::Web::Resize(data().mWeb, width, height);
                return Platform::Web::GetScreenshotImage(data().mWeb);
            }
            return nullptr;
        }
        void SendTouchToWeb(TouchType type, sint32 x, sint32 y) override
        {
            if(IsSigning())
                Platform::Web::SendTouchEvent(data().mWeb, type, x, y);
        }
        void SendKeyToWeb(sint32 code, chars text, bool pressed) override
        {
            if(IsSigning())
                Platform::Web::SendKeyEvent(data().mWeb, code, text, pressed);
        }
        const String& GetName() override
        {
            return data_const().mName;
        }
        const String& GetComment() override
        {
            return data_const().mComment;
        }
        const Image& GetPicture() override
        {
            return data_const().mPicture;
        }
        const Image& GetBackground() override
        {
            return data_const().mBackground;
        }

    private:
        inline Data& data() {return mShare->At<Data>(0);}
        inline const Data& data_const() const {return mShare->At<Data>(0);}

    private:
        static bool OnEvent(payload self, chars type, chars text)
        {
            auto Self = (OAuth2GoogleService*) self;
            if(!String::Compare(type, "UrlChanged"))
            {
                chars GoogleCheckUrl = "http://" "localhost/oauth2callback?";
                static const sint32 GoogleCheckUrlLen = blik_strlen(GoogleCheckUrl);
                if(!String::Compare(text, GoogleCheckUrl, GoogleCheckUrlLen))
                {
                    // 코드 얻기
                    String Code = text + GoogleCheckUrlLen;
                    const sint32 FindBegin = Code.Find(0, "code=");
                    if(FindBegin != -1) Code = Code.Right(Code.Length() - (FindBegin + 5));
                    const sint32 FindEnd = Code.Find(0, "&");
                    if(FindEnd != -1) Code = Code.Left(FindEnd);

                    // 접근토큰/갱신토큰 얻기
                    const String PostData = String::Format(
                        "code=%s&"
                        "client_id=%s&"
                        "client_secret=%s&"
                        "redirect_uri=http://" "localhost/oauth2callback&"
                        "grant_type=authorization_code",
                        (chars) Code, (chars) Self->data_const().mClientId, (chars) Self->data_const().mClientSecret);
                    chars ResultA = AddOn::Curl::RequestString(Self->data().mCurl,
                        "https://" "accounts.google.com/o/oauth2/token", PostData);
                    const Context ResultAJson(ST_Json, SO_OnlyReference, ResultA);
                    Self->data().mAccessToken = ResultAJson("access_token").GetString();
                    Self->data().mRefreshToken = ResultAJson("refresh_token").GetString();

                    // 회원정보 얻기
                    // ※ https://developers.google.com/apis-explorer/?hl=ko#p/plus/v1/ 에 방문하여 슬라이드를 ON으로 변경
                    chars ResultB = AddOn::Curl::RequestString(Self->data().mCurl, String::Format(
                        "https://" "www.googleapis.com/plus/v1/people/me?access_token=%s", (chars) Self->data().mAccessToken));
                    const Context ResultBJson(ST_Json, SO_OnlyReference, ResultB);
                    Self->data().mName = ResultBJson("displayName").GetString();
                    Self->data().mComment = String::Format("팔로워 %d명 - %s",
                        (sint32) ResultBJson("circledByCount").GetInt(), (chars) ResultBJson("tagline").GetString());

                    // 사진 얻기
                    if(chars PictureUrl = ResultBJson("image")("url").GetString(nullptr))
                    {
                        sint32 GetSize = 0;
                        bytes ResultC = AddOn::Curl::RequestBytes(Self->data().mCurl, PictureUrl, &GetSize);
                        id_bitmap NewBitmap = AddOn::Jpg::ToBmp(ResultC, GetSize);
                        Self->data().mPicture.LoadBitmap(NewBitmap);
                        if(Self->data().mPictureClipper)
                            Self->data().mPicture.ReplaceAlphaChannelBy(Self->data().mPictureClipper);
                        Bmp::Remove(NewBitmap);
                    }

                    // 배경 얻기
                    if(chars BackgroundUrl = ResultBJson("cover")("coverPhoto")("url").GetString(nullptr))
                    {
                        sint32 GetSize = 0;
                        bytes ResultC = AddOn::Curl::RequestBytes(Self->data().mCurl, BackgroundUrl, &GetSize);
                        id_bitmap NewBitmap = AddOn::Jpg::ToBmp(ResultC, GetSize);
                        Self->data().mBackground.LoadBitmap(NewBitmap);
                        Bmp::Remove(NewBitmap);
                    }

                    // 웹의 제거
                    Self->data().mNeedDestroyWeb = true;
                    return true;
                }
            }
            return false;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Facebook
    ////////////////////////////////////////////////////////////////////////////////
    class OAuth2FacebookService : public OAuth2Service
    {
    public:
        OAuth2FacebookService()
        {
        }
        ~OAuth2FacebookService() override
        {
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // OAuth2Service
    ////////////////////////////////////////////////////////////////////////////////
    OAuth2Service::OAuth2Service() : mShare(nullptr)
    {
    }

    OAuth2Service::OAuth2Service(const OAuth2Service& rhs) : mShare(nullptr)
    {
        operator=(rhs);
    }

    OAuth2Service::~OAuth2Service()
    {
    }

    OAuth2Service& OAuth2Service::operator=(const OAuth2Service& rhs)
    {
        Share::Remove(mShare);
        if(rhs.mShare) mShare = rhs.mShare->Clone();
        Memory::CopyVPTR(this, &rhs);
        return *this;
    }

    const Strings OAuth2Service::GetValidServices()
    {
        static Strings Services = []()
        {
            Strings Collector;
            Collector.AtAdding() = "Google+";
            Collector.AtAdding() = "Facebook";
            return Collector;
        }();
        return Services;
    }

    OAuth2Service OAuth2Service::Create(chars service)
    {
        if(!String::CompareNoCase(service, "Google+"))
            return OAuth2GoogleService();
        if(!String::CompareNoCase(service, "Facebook"))
            return OAuth2FacebookService();
        return OAuth2Service();
    }
}
