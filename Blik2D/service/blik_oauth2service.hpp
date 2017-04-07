#pragma once
#include <platform/blik_platform.hpp>
#include <element/blik_image.hpp>

namespace BLIK
{
    //! \brief 서비스-OAuth2
    class OAuth2Service
    {
    public:
        OAuth2Service();
        OAuth2Service(const OAuth2Service& rhs);
        virtual ~OAuth2Service();
        OAuth2Service& operator=(const OAuth2Service& rhs);

    // 스타팅함수
    public:
        static const Strings GetValidServices();
        static OAuth2Service Create(chars service);

    // 서비스함수
    public:
        virtual void Signin(chars option, id_bitmap clipper = nullptr) {}
        virtual bool IsSigning() {return false;}
        virtual id_image_read GetWebImage(sint32 width, sint32 height) {return nullptr;}
        virtual void SendTouchToWeb(TouchType type, sint32 x, sint32 y) {}
        virtual void SendKeyToWeb(sint32 code, chars text, bool pressed) {}
        virtual const String& GetName() {static const String _; return _;}
        virtual const String& GetComment() {static const String _; return _;}
        virtual const Image& GetPicture() {static const Image _; return _;}
        virtual const Image& GetBackground() {static const Image _; return _;}

    // 데이터(공유모델)
    protected:
        const Share* mShare;
    };
}
