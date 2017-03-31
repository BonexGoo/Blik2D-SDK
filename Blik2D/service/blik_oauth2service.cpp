#include <blik.hpp>
#include "blik_oauth2service.hpp"

#include <addon/blik_addon.hpp>

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
            Data() {mCurl = nullptr;}
            ~Data() {AddOn::Curl::Release(mCurl);}
            Data(const Data& rhs)
            {
                mCurl = nullptr;
                operator=(rhs);
            }
            Data& operator=(const Data& rhs)
            {
                AddOn::Curl::Release(mCurl);
                mCurl = AddOn::Curl::Clone(rhs.mCurl);
                return *this;
            }
        public:
            id_curl mCurl;
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
        bool IsValid() const override
        {
            return true;
        }
        String GetStartUrl(chars client_id) override
        {
            const String Url = String::Format(
                "https://accounts.google.com/o/oauth2/auth?"
                "client_id=%s&"
                "redirect_uri=urn:ietf:wg:oauth:2.0:oob&"
                "scope=https://www.googleapis.com/auth/youtube&"
                "response_type=code&"
                "access_type=offline", client_id);

            id_curl CurCurl = GetValidCurl();
            String Result;
            AddOn::Curl::Request(CurCurl, Url, &Result, 302);
            return Result;
        }

    private:
        id_curl GetValidCurl()
        {
            Data& CurData = mShare->At<Data>(0);
            if(!CurData.mCurl)
                CurData.mCurl = AddOn::Curl::Create();
            return CurData.mCurl;
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

    public:
        bool IsValid() const override
        {
            return true;
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
