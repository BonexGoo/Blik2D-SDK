#include <blik.hpp>
#include "blik_point.hpp"

namespace BLIK
{
    Point::Point()
    {
        x = 0;
        y = 0;
    }

    Point::Point(const Point& rhs)
    {
        operator=(rhs);
    }

    Point::Point(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    Point::~Point()
    {
    }

    Point& Point::operator=(const Point& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }

    Point& Point::operator*=(const Matrix& rhs)
    {
        const float New_x = x * rhs.m11 + y * rhs.m21 + rhs.dx;
        const float New_y = x * rhs.m12 + y * rhs.m22 + rhs.dy;
        x = New_x;
        y = New_y;
        return *this;
    }

    Point Point::operator*(const Matrix& rhs) const
    {
        return Point(*this).operator*=(rhs);
    }

    Point& Point::operator*=(const float rhs)
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    Point Point::operator*(const float rhs) const
    {
        return Point(*this).operator*=(rhs);
    }

    Point& Point::operator*=(const Size& rhs)
    {
        x *= rhs.w;
        y *= rhs.h;
        return *this;
    }

    Point Point::operator*(const Size& rhs) const
    {
        return Point(*this).operator*=(rhs);
    }

    Point& Point::operator/=(const float rhs)
    {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    Point Point::operator/(const float rhs) const
    {
        return Point(*this).operator/=(rhs);
    }

    Point& Point::operator+=(const Point& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Point Point::operator+(const Point& rhs) const
    {
        return Point(*this).operator+=(rhs);
    }

    Point& Point::operator+=(const Size& rhs)
    {
        x += rhs.w;
        y += rhs.h;
        return *this;
    }

    Point Point::operator+(const Size& rhs) const
    {
        return Point(*this).operator+=(rhs);
    }

    Point& Point::operator-=(const Point& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Point Point::operator-(const Point& rhs) const
    {
        return Point(*this).operator-=(rhs);
    }

    Point& Point::operator-=(const Size& rhs)
    {
        x -= rhs.w;
        y -= rhs.h;
        return *this;
    }

    Point Point::operator-(const Size& rhs) const
    {
        return Point(*this).operator-=(rhs);
    }

    bool Point::operator==(const Point& rhs) const
    {
        return (x == rhs.x && y == rhs.y);
    }

    bool Point::operator!=(const Point& rhs) const
    {
        return !operator==(rhs);
    }

    const bool Point::ClockwiseTest(const Point begin, const Point end) const
    {
        return (0 < (end.x - begin.x) * (y - begin.y) - (end.y - begin.y) * (x - begin.x));
    }
}
