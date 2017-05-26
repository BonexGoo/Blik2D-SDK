#include <blik.hpp>
#include "blik_oauth2service.hpp"

#include <addon/blik_addon.hpp>
#include <element/blik_image.hpp>
#include <format/blik_bmp.hpp>

namespace BLIK
{
    class OAuth2ServiceImpl : public OAuth2Service
    {
    protected:
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
            String mServiceId;
            String mName;
            String mComment;
            Image mPicture;
            Image mBackground;
            id_bitmap mPictureClipper;
        };

    protected:
        OAuth2ServiceImpl(buffer gift)
        {
            mShare = Share::Create(gift);
        }
        ~OAuth2ServiceImpl() override
        {
        }

    private:
        void Signin(chars option, id_bitmap clipper) override
        {
            Context ClientInfo(ST_Json, SO_OnlyReference, option);
            data().mClientId = ClientInfo[0].GetString();
            data().mClientSecret = ClientInfo[1].GetString();
            Bmp::Remove(data().mPictureClipper);
            data().mPictureClipper = clipper;

            String ResultUrl = AddOn::Curl::RequestRedirectUrl(data().mCurl, SigninCore(data_const().mClientId), 302);
            data().mWeb = Platform::Web::Create(ResultUrl, 0, 0, OnEvent, (payload) this);
            data().mNeedDestroyWeb = false;
        }
        bool IsSigning(bool needDestroy, bool* destroyResult = nullptr) override
        {
            if(needDestroy)
            {
                if(data().mNeedDestroyWeb)
                {
                    data().mNeedDestroyWeb = false;
                    Platform::Web::Release(data().mWeb);
                    if(destroyResult) *destroyResult = true;
                    return false;
                }
                else if(destroyResult) *destroyResult = false;
            }
            return (data_const().mWeb.get() != nullptr);
        }
        id_image_read GetWebImage(sint32 width, sint32 height) override
        {
            if(IsSigning(false))
            {
                Platform::Web::Resize(data().mWeb, width, height);
                return Platform::Web::GetScreenshotImage(data().mWeb);
            }
            return nullptr;
        }
        void SendTouchToWeb(TouchType type, sint32 x, sint32 y) override
        {
            if(IsSigning(false))
                Platform::Web::SendTouchEvent(data().mWeb, type, x, y);
        }
        void SendKeyToWeb(sint32 code, chars text, bool pressed) override
        {
            if(IsSigning(false))
                Platform::Web::SendKeyEvent(data().mWeb, code, text, pressed);
        }
        chars GetServiceName() const override
        {
            return service_name();
        }
        const String& GetServiceId() const override
        {
            return data_const().mServiceId;
        }
        const String& GetName() const override
        {
            return data_const().mName;
        }
        const String& GetComment() const override
        {
            return data_const().mComment;
        }
        const Image& GetPicture() const override
        {
            return data_const().mPicture;
        }
        const Image& GetBackground() const override
        {
            return data_const().mBackground;
        }

    private:
        static bool OnEvent(payload self, chars type, chars text)
        {
            auto Self = (OAuth2ServiceImpl*) self;
            if(!String::Compare(type, "UrlChanged"))
            {
                chars CallbackUrl = "http://" "localhost/oauth2callback?";
                static const sint32 CallbackUrlLen = blik_strlen(CallbackUrl);
                if(!String::Compare(text, CallbackUrl, CallbackUrlLen))
                {
                    // 코드 얻기
                    String Code = text + CallbackUrlLen;
                    const sint32 FindBegin = Code.Find(0, "code=");
                    if(FindBegin != -1) Code = Code.Right(Code.Length() - (FindBegin + 5));
                    const sint32 FindEnd = Code.Find(0, "&");
                    if(FindEnd != -1) Code = Code.Left(FindEnd);

                    Self->OnEventCore(Code);

                    // 웹의 제거
                    Self->data().mNeedDestroyWeb = true;
                    return true;
                }
            }
            return false;
        }

    protected:
        void ReloadPicture(chars url)
        {
            if(url)
            {
                sint32 GetSize = 0;
                bytes Result = AddOn::Curl::RequestBytes(data().mCurl, url, &GetSize);
                id_bitmap NewBitmap = AddOn::Jpg::ToBmp(Result, GetSize);
                data().mPicture.LoadBitmap(NewBitmap);
                if(data().mPictureClipper)
                    data().mPicture.ReplaceAlphaChannelBy(data().mPictureClipper);
                Bmp::Remove(NewBitmap);
            }
        }
        void ReloadBackground(chars url)
        {
            if(url)
            {
                sint32 GetSize = 0;
                bytes Result = AddOn::Curl::RequestBytes(data().mCurl, url, &GetSize);
                id_bitmap NewBitmap = AddOn::Jpg::ToBmp(Result, GetSize);
                data().mBackground.LoadBitmap(NewBitmap);
                Bmp::Remove(NewBitmap);
            }
        }

    private:
        virtual String SigninCore(chars clientId) = 0;
        virtual void OnEventCore(chars code) = 0;

    protected:
        inline Data& data() {return mShare->At<Data>(0);}
        inline const Data& data_const() const {return mShare->At<Data>(0);}
        virtual chars service_name() const = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Google+
    ////////////////////////////////////////////////////////////////////////////////
    class OAuth2GoogleService : public OAuth2ServiceImpl
    {
    private:
        chars service_name() const override {return "google";}

    public:
        OAuth2GoogleService() : OAuth2ServiceImpl(Buffer::Alloc<Data>(BLIK_DBG 1))
        {
        }
        ~OAuth2GoogleService() override
        {
        }

    private:
        String SigninCore(chars clientId) override
        {
            return String::Format(
                "https://" "accounts.google.com/o/oauth2/auth?"
                "client_id=%s&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "scope=https://" "www.googleapis.com/auth/plus.me&"
                "response_type=code&"
                "access_type=offline", clientId);
        }
        void OnEventCore(chars code) override
        {
            // 접근토큰/갱신토큰 얻기
            const String PostData = String::Format(
                "code=%s&"
                "client_id=%s&"
                "client_secret=%s&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "grant_type=authorization_code",
                code, (chars) data_const().mClientId, (chars) data_const().mClientSecret);
            chars ResultA = AddOn::Curl::RequestString(data().mCurl,
                "https://" "accounts.google.com/o/oauth2/token", PostData);
            const Context ResultAJson(ST_Json, SO_OnlyReference, ResultA);
            data().mAccessToken = ResultAJson("access_token").GetString();
            data().mRefreshToken = ResultAJson("refresh_token").GetString();

            // 회원정보 얻기(https://developers.google.com/apis-explorer/?hl=ko#p/plus/v1/ 에 방문하여 슬라이드를 ON으로 변경)
            chars ResultB = AddOn::Curl::RequestString(data().mCurl, String::Format(
                "https://" "www.googleapis.com/plus/v1/people/me?access_token=%s",
                (chars) data_const().mAccessToken));
            const Context ResultBJson(ST_Json, SO_OnlyReference, ResultB);
            data().mName = ResultBJson("displayName").GetString();
            data().mServiceId = String("google_") + ResultBJson("id").GetString();
            data().mComment = String::Format("팔로워 %d명 - %s",
                (sint32) ResultBJson("circledByCount").GetInt(), (chars) ResultBJson("tagline").GetString());

            // 사진/배경 얻기
            ReloadPicture(ResultBJson("image")("url").GetString(nullptr));
            ReloadBackground(ResultBJson("cover")("coverPhoto")("url").GetString(nullptr));
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Facebook
    ////////////////////////////////////////////////////////////////////////////////
    class OAuth2FacebookService : public OAuth2ServiceImpl
    {
    private:
        chars service_name() const override {return "facebook";}

    public:
        OAuth2FacebookService() : OAuth2ServiceImpl(Buffer::Alloc<Data>(BLIK_DBG 1))
        {
        }
        ~OAuth2FacebookService() override
        {
        }

    private:
        String SigninCore(chars clientId) override
        {
            // 스코프는 App Review에서 변경, developers.facebook.com/apps/[앱ID]/review-status
            return String::Format(
                "https://" "www.facebook.com/dialog/oauth?"
                "client_id=%s&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "scope=public_profile,user_photos", clientId);
        }
        void OnEventCore(chars code) override
        {
            // 접근토큰 얻기
            const String Url = String::Format(
                "https://" "graph.facebook.com/oauth/access_token?"
                "client_id=%s&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "client_secret=%s&"
                "code=%s",
                (chars) data_const().mClientId, (chars) data_const().mClientSecret, code);
            chars ResultA = AddOn::Curl::RequestString(data().mCurl, Url);
            const Context ResultAJson(ST_Json, SO_OnlyReference, ResultA);
            data().mAccessToken = ResultAJson("access_token").GetString();

            // 회원정보 얻기
            chars ResultB = AddOn::Curl::RequestString(data().mCurl, String::Format(
                "https://" "graph.facebook.com/me?access_token=%s&"
                "fields=id,name,picture,cover,context",
                (chars) data().mAccessToken));
            const Context ResultBJson(ST_Json, SO_OnlyReference, ResultB);
            data().mName = ResultBJson("name").GetString();
            data().mServiceId = String("facebook_") + ResultBJson("id").GetString();
            data().mComment = String::Format("좋아요 총 %d건",
                (sint32) ResultBJson("context")("mutual_likes")("summary")("total_count").GetInt());

            // 사진/배경 얻기
            ReloadPicture(ResultBJson("picture")("data")("url").GetString(nullptr));
            ReloadBackground(ResultBJson("cover")("source").GetString(nullptr));
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Kakao
    ////////////////////////////////////////////////////////////////////////////////
    class OAuth2KakaoService : public OAuth2ServiceImpl
    {
    private:
        chars service_name() const override {return "kakao";}

    public:
        OAuth2KakaoService() : OAuth2ServiceImpl(Buffer::Alloc<Data>(BLIK_DBG 1))
        {
        }
        ~OAuth2KakaoService() override
        {
        }

    private:
        String SigninCore(chars clientId) override
        {
            // 사이트 도메인과 Redirect Path를 선행하여 수정할 것(https://developers.kakao.com/apps/[앱번호]/settings/general)
            return String::Format(
                "https://" "kauth.kakao.com/oauth/authorize?"
                "client_id=%s&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "response_type=code", clientId);
        }
        void OnEventCore(chars code) override
        {
            // 접근토큰 얻기
            const String PostData = String::Format(
                "grant_type=authorization_code&"
                "client_id=%s&"
                "redirect_uri=http://" "localhost/oauth2callback&"
                "code=%s",
                (chars) data_const().mClientId, code);
            chars ResultA = AddOn::Curl::RequestString(data().mCurl,
                "https://" "kauth.kakao.com/oauth/token", PostData);
            const Context ResultAJson(ST_Json, SO_OnlyReference, ResultA);
            data().mAccessToken = ResultAJson("access_token").GetString();
            data().mRefreshToken = ResultAJson("refresh_token").GetString();

            // 회원정보 얻기
            chars ResultB = AddOn::Curl::RequestString(data().mCurl,
                "https://" "kapi.kakao.com/v1/api/story/profile", nullptr,
                String::Format("Authorization: Bearer %s", (chars) data().mAccessToken));
            const Context ResultBJson(ST_Json, SO_OnlyReference, ResultB);
            data().mName = ResultBJson("nickName").GetString();
            chars BirthType = "";
            if(!String::Compare(ResultBJson("birthdayType").GetString(), "SOLAR")) BirthType = "(양력)";
            else if(!String::Compare(ResultBJson("birthdayType").GetString(), "LUNAR")) BirthType = "(음력)";
            const sint32 BirthDay = (sint32) ResultBJson("birthday").GetInt();
            data().mComment = String::Format("생일%s %d월 %d일", BirthType, BirthDay / 100, BirthDay % 100);

            // 사진/배경 얻기
            ReloadPicture(ResultBJson("thumbnailURL").GetString(nullptr));
            ReloadBackground(ResultBJson("bgImageURL").GetString(nullptr));

            // ID정보 얻기
            chars ResultC = AddOn::Curl::RequestString(data().mCurl,
                "https://" "kapi.kakao.com/v1/user/me", nullptr,
                String::Format("Authorization: Bearer %s", (chars) data().mAccessToken));
            const Context ResultCJson(ST_Json, SO_OnlyReference, ResultC);
            data().mServiceId = String("kakao_") + ResultCJson("id").GetString();
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
        Share::Remove(mShare);
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
            Collector.AtAdding() = "Kakao";
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
        if(!String::CompareNoCase(service, "Kakao"))
            return OAuth2KakaoService();
        return OAuth2Service();
    }
}
