#pragma once
#define BLIK_NOT_INCLUDE_FOR_INTELLIGENCE
#include <blik.hpp>

namespace BLIK
{
    //! \brief 메모리함수지원
    class Memory
    {
    public:
        /*!
        \brief 할당
        \param size : 할당할 길이(byte)
        \return 할당된 시작주소
        */
        static void* Alloc(const sint32 size);

        /*!
        \brief 해제
        \param ptr : 해제할 시작주소
        */
        static void Free(void* ptr);

        /*!
        \brief 초기화
        \param ptr : 초기화할 시작주소
        \param value : 초기화할 값
        \param size : 초기화할 길이(byte)
        */
        static void Set(void* ptr, const uint08 value, const sint32 size);

        /*!
        \brief 복사
        \param dst : 복사받을 시작주소
        \param src : 복사할 시작주소
        \param size : 복사할 길이(byte)
        */
        static void Copy(void* dst, const void* src, const sint32 size);

        /*!
        \brief 가상함수테이블포인터 복사
        \param dst : 복사받을 객체의 시작주소
        \param src : 복사할 객체의 시작주소
        */
        static void CopyVPTR(void* dst, const void* src);

        /*!
        \brief 비교
        \param dst : 비교당할 시작주소
        \param src : 비교할 시작주소
        \param size : 비교할 길이(byte)
        \return 비교결과(0-같음)
        */
        static sint32 Compare(const void* dst, const void* src, const sint32 size);
    };
}
