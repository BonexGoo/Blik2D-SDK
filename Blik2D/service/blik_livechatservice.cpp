#include <blik.hpp>
#include "blik_livechatservice.hpp"

namespace BLIK
{
    ////////////////////////////////////////////////////////////////////////////////
    // LiveChatData
    ////////////////////////////////////////////////////////////////////////////////
    class LiveChatData
    {
    public:
        LiveChatData()
        {
        }
        ~LiveChatData()
        {
            while(LiveChatService::Message* NewMessage = mMessages.Dequeue())
                NewMessage->DestroyMe();
        }
        LiveChatData(const LiveChatData& rhs) {operator=(rhs);}
        LiveChatData& operator=(const LiveChatData& rhs)
        {BLIK_ASSERT("This class is not allowed to be copied", false); return *this;}

    public:
        class UserInfo
        {
        public:
            UserInfo() {}
            ~UserInfo() {}
        public:
            String mName;
            Image mPicture;
        };

    public:
        Queue<LiveChatService::Message*> mMessages;
        Map<UserInfo> mUserInfos;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // LiveChatService
    ////////////////////////////////////////////////////////////////////////////////
    #define mDATA (mShare->At<LiveChatData>(0))

    LiveChatService::LiveChatService() : mShare(Share::Create(Buffer::Alloc<LiveChatData>(BLIK_DBG 1)))
    {
    }

    LiveChatService::LiveChatService(const LiveChatService& rhs) : mShare(nullptr)
    {
        operator=(rhs);
    }

    LiveChatService::~LiveChatService()
    {
        Share::Remove(mShare);
    }

    LiveChatService& LiveChatService::operator=(const LiveChatService& rhs)
    {
        Share::Remove(mShare);
        if(rhs.mShare) mShare = rhs.mShare->Clone();
        return *this;
    }

    const Strings LiveChatService::GetValidServices()
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

    void LiveChatService::AddService(chars service, chars key)
    {
    }

    void LiveChatService::SubService(chars service, chars key)
    {
    }

    sint32 LiveChatService::TryNextMessage(String& text)
    {
        sint32 Result = 0;
        Message* NewMessage = mDATA.mMessages.Dequeue();
        if(NewMessage)
        {
            Result = NewMessage->mUserId;
            text = NewMessage->mText;
            NewMessage->DestroyMe();
        }
        return Result;
    }

    const String& LiveChatService::GetName(sint32 userid)
    {
        return mDATA.mUserInfos[userid].mName;
    }

    const Image& LiveChatService::GetPicture(sint32 userid)
    {
        return mDATA.mUserInfos[userid].mPicture;
    }
}
