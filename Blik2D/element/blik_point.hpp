#pragma once
#include <element/blik_matrix.hpp>
#include <element/blik_size.hpp>

namespace BLIK
{
    //! \brief 2D좌표
    class Point
    {
    public:
        Point();
        Point(const Point& rhs);
        Point(float x, float y);
        ~Point();

        Point& operator=(const Point& rhs);
        Point& operator*=(const Matrix& rhs);
        Point operator*(const Matrix& rhs) const;
        Point& operator*=(const float rhs);
        Point operator*(const float rhs) const;
		Point& operator*=(const Size& rhs);
        Point operator*(const Size& rhs) const;
        Point& operator/=(const float rhs);
        Point operator/(const float rhs) const;
        Point& operator+=(const Point& rhs);
        Point operator+(const Point& rhs) const;
        Point& operator+=(const Size& rhs);
        Point operator+(const Size& rhs) const;
        Point& operator-=(const Point& rhs);
        Point operator-(const Point& rhs) const;
		Point& operator-=(const Size& rhs);
        Point operator-(const Size& rhs) const;
        bool operator==(const Point& rhs) const;
        bool operator!=(const Point& rhs) const;

        const bool ClockwiseTest(const Point begin, const Point end) const;

    public:
        float x;
        float y;
    };
    typedef Array<Point, datatype_class_canmemcpy> Points;
}
