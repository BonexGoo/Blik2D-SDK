#pragma once
#include <blik_share.hpp>

namespace BLIK
{
    //! \brief 공유객체지원
    template<typename TYPE, datatype DATATYPE = datatype_class_nomemcpy, bool BASEALLOC = false>
    class Object
    {
    public:
        /*!
        \brief 비우기
        */
        void Clear()
        {
            Share::Remove(share);
            share = nullptr;
        }

        /*!
        \brief 공유ID 추출
        \return 공유ID
        */
        operator id_share() const {return (id_share) share;}

        /*!
        \brief 공유ID 추출(복제방식)
        \return 공유ID
        */
        operator id_cloned_share() const {return (id_cloned_share) share->Clone();}

        /*!
        \brief 복사
        \param rhs : 복사할 인스턴스
        \return 자기 객체
        */
        Object& operator=(const Object& rhs)
        {
            Share::Remove(share);
            share = (rhs.share)? rhs.share->Clone() : nullptr;
            return *this;
        }

        /*!
        \brief 복사
        \param rhs : 복사할 인스턴스
        \return 자기 객체
        */
        Object& operator=(const TYPE& rhs)
        {
            Share::Remove(share);
            share = Share::Create(Buffer::Alloc<TYPE, DATATYPE>(BLIK_DBG 1));
            share->At<TYPE>(0) = rhs;
            return *this;
        }

        /*!
        \brief 인수(버퍼로부터)
        \param rhs : 인수할 버퍼
        \return 자기 객체
        */
        Object& operator=(buffer rhs)
        {
            Share::Remove(share);
            share = Share::Create(rhs);
            return *this;
        }

        /*!
        \brief 인수(공유ID로부터)
        \param rhs : 복사할 공유ID
        \return 자기 객체
        */
        Object& operator=(id_share_read rhs)
        {
            BLIK_ASSERT("공유ID가 배열로 구성되어서 복사할 수 없습니다", ((Share*) rhs)->count() == 1);
            Share::Remove(share);
            share = ((Share*) rhs)->Clone();
            return *this;
        }

        /*!
        \brief 인수(복제된 공유ID로부터)
        \param rhs : 복사할 공유ID
        \return 자기 객체
        */
        Object& operator=(id_cloned_share_read rhs)
        {
            BLIK_ASSERT("공유ID가 배열로 구성되어서 복사할 수 없습니다", ((Share*) rhs)->count() == 1);
            Share::Remove(share);
            share = (Share*) rhs;
            return *this;
        }

        /*!
        \brief 타입명 구하기
        \return 타입명
        */
        chars Type() const
        {
            return share->Type();
        }

        /*!
        \brief 타입ID 구하기
        \return 타입ID
        */
        sblock TypeID() const
        {
            return share->TypeID();
        }

        /*!
        \brief 멤버접근
        \return 자기 타입
        */
        TYPE* operator->()
        {
            BLIK_ASSERT("빈 객체입니다", share);
            return Ptr();
        }

        /*!
        \brief 멤버접근(읽기전용)
        \return 자기 타입
        */
        TYPE const* operator->() const
        {
            BLIK_ASSERT("빈 객체입니다", share);
            return ConstPtr();
        }

        /*!
        \brief 데이터값
        \return 데이터
        */
        TYPE& Value()
        {
            BLIK_ASSERT("빈 객체입니다", share);
            share = Share::CopyOnWrite(share, 0);
            return share->At<TYPE>(0);
        }

        /*!
        \brief 데이터값(읽기전용)
        \return 데이터
        */
        TYPE const& ConstValue() const
        {
            BLIK_ASSERT("빈 객체입니다", share);
            return share->At<TYPE>(0);
        }

        /*!
        \brief 데이터값(공유방식)
        \return 데이터
        */
        TYPE& SharedValue()
        {
            if(!share)
                share = Share::Create(Buffer::Alloc<TYPE, DATATYPE>(BLIK_DBG 1));
            return share->At<TYPE>(0);
        }

        /*!
        \brief 주소값
        \return 주소
        */
        TYPE* Ptr()
        {
            if(!share) return nullptr;
            share = Share::CopyOnWrite(share, 0);
            return &share->At<TYPE>(0);
        }

        /*!
        \brief 주소값(읽기전용)
        \return 주소
        */
        TYPE const* ConstPtr() const
        {
            if(!share) return nullptr;
            return &share->At<TYPE>(0);
        }

        /*!
        \brief 버퍼생성
        \return 버퍼
        */
        buffer CopiedBuffer() const
        {
            if(!share) return nullptr;
            return share->CopiedBuffer();
        }

        /*!
        \brief 생성자
        \param doalloc : 버퍼 생성여부
        */
        Object(bool doAlloc = BASEALLOC) : share((doAlloc)? Share::Create(Buffer::Alloc<TYPE, DATATYPE>(BLIK_DBG 1)) : nullptr) {}

        /*!
        \brief 생성자
        \param rhs : 복사할 인스턴스
        */
        Object(const Object& rhs) : share(nullptr) {operator=(rhs);}

        /*!
        \brief 생성자
        \param rhs : 복사할 TYPE 인스턴스
        */
        Object(const TYPE& rhs) : share(nullptr) {operator=(rhs);}

        /*!
        \brief 생성자(버퍼로부터)
        \param rhs : 인수할 버퍼
        */
        Object(buffer rhs) : share(nullptr) {operator=(rhs);}

        /*!
        \brief 생성자(공유ID로부터)
        \param rhs : 복사할 공유ID
        */
        Object(id_share_read rhs) : share(nullptr) {operator=(rhs);}

        /*!
        \brief 생성자(복제된 공유ID로부터)
        \param rhs : 복사할 공유ID
        */
        Object(id_cloned_share_read rhs) : share(nullptr) {operator=(rhs);}

        /*!
        \brief 소멸자
        */
        ~Object() {Share::Remove(share);}

    private:
        const Share* share;
    };
}
