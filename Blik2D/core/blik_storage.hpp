#pragma once
#define BLIK_NOT_INCLUDE_FOR_INTELLIGENCE
#include <blik.hpp>

#define BLIK_STORAGE(CLASS, ...) \
    [&]()->CLASS* {static id_storage NewStorage = \
    Storage::Create([](void* ptr)->void {delete (CLASS*) ptr;}); \
    CLASS** Result = (CLASS**) Storage::Bind(NewStorage); \
    return (*Result)? *Result : (*Result = new CLASS(__VA_ARGS__));}()
#define BLIK_STORAGE_SYS(CLASS, ...) \
    [&]()->CLASS* {static id_storage NewStorage = \
    Storage::Create([](void* ptr)->void {delete (CLASS*) ptr;}, CT_System); \
    CLASS** Result = (CLASS**) Storage::Bind(NewStorage); \
    return (*Result)? *Result : (*Result = new CLASS(__VA_ARGS__));}()

namespace BLIK
{
    BLIK_DECLARE_ID(id_storage);
    enum ClearType {CT_Null, CT_System, CT_User};
    enum ClearLevel {CL_SystemAndUser, CL_UserOnly};

    //! \brief 스토리지(TLS)지원
    class Storage
    {
    public:
        typedef void (*DeleteCB)(void*);

    public:
        /*!
        \brief 스토리지를 생성
        \param cb : 객체소멸 콜백함수
        \param type : 인스턴스 소멸을 위한 분류타입
        \return 스토리지ID
        \see Bind, Clear, ClearAll
        */
        static id_storage Create(DeleteCB cb, ClearType type = CT_User);

        /*!
        \brief 해당 스토리지에 스레드별 인스턴스를 연동(생성)
        \param storage : 스토리지ID
        \return 인스턴스의 주소
        \see Create
        */
        static void** Bind(id_storage storage);

        /*!
        \brief 해당 스토리지에 연동된 인스턴스를 소멸
        \param storage : 스토리지ID
        \return 소멸시행 여부
        \see ClearAll
        */
        static bool Clear(id_storage storage);

        /*!
        \brief 모든 스토리지에 연동된 인스턴스를 소멸
        \param level : 소멸수준
        \return 소멸시행 수량
        \see Bind, Clear
        */
        static sint32 ClearAll(ClearLevel level = CL_UserOnly);
    };
}
