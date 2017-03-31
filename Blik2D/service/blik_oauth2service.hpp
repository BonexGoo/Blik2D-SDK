#pragma once
#include <platform/blik_platform.hpp>

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
        virtual bool IsValid() const {return false;}
        virtual String GetStartUrl(chars client_id) {return "";}

    // 데이터(공유모델)
    protected:
        const Share* mShare;
    };
}
