#pragma once
#include <element/blik_point.hpp>
#include <element/blik_size.hpp>

namespace BLIK
{
    //! \brief 2D영역
    class Rect
    {
    public:
        Rect();
        Rect(const Rect& rhs);
        Rect(const Point point, const Size size);
        Rect(const Point point1, const Point point2);
        Rect(float l, float t, float r, float b);
        ~Rect();

        Rect& operator=(const Rect& rhs);
        Rect& operator+=(const Point& rhs);
        Rect operator+(const Point& rhs) const;
        Rect& operator-=(const Point& rhs);
        Rect operator-(const Point& rhs) const;
        Rect& operator+=(const Size& rhs);
        Rect operator+(const Size& rhs) const;
        Rect& operator-=(const Size& rhs);
        Rect operator-(const Size& rhs) const;
        Rect& operator*=(float rhs);
        Rect operator*(float rhs) const;
        Rect& operator/=(float rhs);
        Rect operator/(float rhs) const;
        bool operator==(const Rect& rhs) const;
        bool operator!=(const Rect& rhs) const;

        inline const float Width() const {return r - l;}
        inline const float Height() const {return b - t;}
        inline const float CenterX() const {return (r + l) / 2;}
        inline const float CenterY() const {return (b + t) / 2;}
        const bool PtInRect(const Point point) const;
        const bool PtInRect(const float x, const float y) const;
        const bool ContactTest(const Rect rect) const;
        Rect Inflate(const float x, const float y) const;
        Rect Deflate(const float x, const float y) const;

    public:
        float l;
        float t;
        float r;
        float b;
    };
    typedef Array<Rect, datatype_class_canmemcpy> Rects;
}
