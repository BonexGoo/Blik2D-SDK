#pragma once
#include <element/blik_point.hpp>

namespace BLIK
{
    //! \brief 2D벡터
    class Vector
    {
    public:
        Vector();
        Vector(const Vector& rhs);
        Vector(float x, float y, float vx, float vy);
        Vector(const Point& pos, const Point& vec);
        ~Vector();

        Vector& operator=(const Vector& rhs);
        Vector& operator+=(const Point& rhs);
        Vector operator+(const Point& rhs) const;
        Vector& operator-=(const Point& rhs);
        Vector operator-(const Point& rhs) const;
        bool operator==(const Vector& rhs) const;
        bool operator!=(const Vector& rhs) const;

    public:
        float x;
        float y;
        float vx;
        float vy;
    };
    typedef Array<Vector, datatype_class_canmemcpy> Vectors;
}
