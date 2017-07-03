#pragma once
#include <element/blik_image.hpp>

namespace BLIK
{
    //! \brief 서비스-LiveChat
    class LiveChatService
    {
    public:
        class Message
        {
        public:
            Message() {mUserId = 0;}
            Message(const Message& rhs) {operator=(rhs);}
            ~Message() {}
            Message& operator=(const Message& rhs)
            {
                mUserId = rhs.mUserId;
                mText = rhs.mText;
                return *this;
            }

        public:
            inline void DestroyMe()
            {Buffer::Free((buffer) this);}

        public:
            sint32 mUserId;
            String mText;
        };

    public:
        LiveChatService();
        LiveChatService(const LiveChatService& rhs);
        virtual ~LiveChatService();
        LiveChatService& operator=(const LiveChatService& rhs);

    // 정적함수
    public:
        static const Strings GetValidServices();

    // 서비스함수
    public:
        void AddService(chars service, chars key);
        void SubService(chars service, chars key);
        sint32 TryNextMessage(String& text);
        const String& GetName(sint32 userid);
        const Image& GetPicture(sint32 userid);

    // 데이터(공유모델)
    protected:
        const Share* mShare;
    };
    typedef Array<LiveChatService::Message> LiveChatMessages;
}
