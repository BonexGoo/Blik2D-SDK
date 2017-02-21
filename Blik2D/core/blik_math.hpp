#pragma once
#define BLIK_NOT_INCLUDE_FOR_INTELLIGENCE
#include <blik.hpp>

namespace BLIK
{
    //! \brief 수학함수지원
    class Math
    {
    public:
        /*!
        \brief 절대값 구하기
        \param v : 인수
        \return 절대값
        */
        inline static sint32 Abs(const sint32 v)
        {return (v < 0)? -v : v;}

        /*!
        \brief 절대값 구하기
        \param v : 인수
        \return 절대값
        */
        inline static float AbsF(const float v)
        {return (v < 0)? -v : v;}

        /*!
        \brief 최소값 구하기
        \param a : 비교A
        \param b : 비교B
        \return 최소값
        \see Max
        */
        inline static sint32 Min(const sint32 a, const sint32 b)
        {return (a < b)? a : b;}

        /*!
        \brief 최소값 구하기(float)
        \param a : 비교A
        \param b : 비교B
        \return 최소값
        \see MaxF
        */
        static float MinF(const float a, const float b);

        /*!
        \brief 최대값 구하기
        \param a : 비교A
        \param b : 비교B
        \return 최대값
        \see Min
        */
        inline static sint32 Max(const sint32 a, const sint32 b)
        {return (a > b)? a : b;}

        /*!
        \brief 최대값 구하기(float)
        \param a : 비교A
        \param b : 비교B
        \return 최대값
        \see MinF
        */
        static float MaxF(const float a, const float b);

		/*!
        \brief 올림값 구하기
        \param v : 인수
        \return 올림값
        */
		static float Ceil(const float v);

		/*!
        \brief 내림값 구하기
        \param v : 인수
        \return 내림값
        */
		static float Floor(const float v);

		/*!
        \brief 반올림값 구하기
        \param v : 인수
        \return 반올림값
        */
		static float Round(const float v);

        /*!
        \brief 나머지 구하기
        \param a : 피제수
        \param b : 제수
        \return 나머지
        */
        static float Mod(const float a, const float b);

        /*!
        \brief 제곱근 구하기
        \param v : 인수
        \return 제곱근
        */
        static float Sqrt(const float v);

        /*!
        \brief 코사인 구하기
        \param v : 인수
        \return 코사인
        */
        static float Cos(const float v);

        /*!
        \brief 사인 구하기
        \param v : 인수
        \return 사인
        */
        static float Sin(const float v);

        /*!
        \brief 아크탄젠트 구하기
        \param x : 변화량X
        \param y : 변화량Y
        \return 아크탄젠트
        */
        static float Atan(const float x, const float y);

        /*!
        \brief 각도를 라디안으로 변환하기
        \param deg : 각도
        \return 라디안
        */
        inline static float ToRadian(const float deg)
        {return deg * 3.1415926535897932385f / 180.0f;}

        /*!
        \brief 라디안을 각도로 변환하기
        \param rad : 라디안
        \return 각도
        */
        inline static float ToDegree(const float rad)
        {return rad * 180.0f / 3.1415926535897932385f;}

        /*!
        \brief 거리 구하기
        \param x1 : 좌표X1
        \param y1 : 좌표Y1
        \param x2 : 좌표X2
        \param y2 : 좌표Y2
        \return 거리
        */
        static float Distance(const float x1, const float y1, const float x2, const float y2);
    };
}
