#include <blik.hpp>
#include "blik_rect.hpp"

namespace BLIK
{
    Rect::Rect()
    {
        l = 0;
        t = 0;
        r = 0;
        b = 0;
    }

    Rect::Rect(const Rect& rhs)
    {
        operator=(rhs);
    }

    Rect::Rect(const Point point, const Size size)
    {
        l = point.x;
        t = point.y;
        r = point.x + size.w;
        b = point.y + size.h;
    }

    Rect::Rect(const Point point1, const Point point2)
    {
        l = (point1.x < point2.x)? point1.x : point2.x;
        t = (point1.y < point2.y)? point1.y : point2.y;
        r = (point1.x < point2.x)? point2.x : point1.x;
        b = (point1.y < point2.y)? point2.y : point1.y;
    }

    Rect::Rect(float l, float t, float r, float b)
    {
        this->l = (l < r)? l : r;
        this->t = (t < b)? t : b;
        this->r = (l < r)? r : l;
        this->b = (t < b)? b : t;
    }

    Rect::~Rect()
    {
    }

    Rect& Rect::operator=(const Rect& rhs)
    {
        l = rhs.l;
        t = rhs.t;
        r = rhs.r;
        b = rhs.b;
        return *this;
    }

    Rect& Rect::operator+=(const Point& rhs)
    {
        l += rhs.x;
        t += rhs.y;
        r += rhs.x;
        b += rhs.y;
        return *this;
    }

    Rect Rect::operator+(const Point& rhs) const
    {
        return Rect(*this).operator+=(rhs);
    }

    Rect& Rect::operator-=(const Point& rhs)
    {
        l -= rhs.x;
        t -= rhs.y;
        r -= rhs.x;
        b -= rhs.y;
        return *this;
    }

    Rect Rect::operator-(const Point& rhs) const
    {
        return Rect(*this).operator-=(rhs);
    }

    Rect& Rect::operator+=(const Size& rhs)
    {
        l -= rhs.w;
        t -= rhs.h;
        r += rhs.w;
        b += rhs.h;
        return *this;
    }

    Rect Rect::operator+(const Size& rhs) const
    {
        return Rect(*this).operator+=(rhs);
    }

    Rect& Rect::operator-=(const Size& rhs)
    {
        l += rhs.w;
        t += rhs.h;
        r -= rhs.w;
        b -= rhs.h;
        return *this;
    }

    Rect Rect::operator-(const Size& rhs) const
    {
        return Rect(*this).operator-=(rhs);
    }

    Rect& Rect::operator*=(float rhs)
    {
        l *= rhs;
        t *= rhs;
        r *= rhs;
        b *= rhs;
        return *this;
    }

    Rect Rect::operator*(float rhs) const
    {
        return Rect(*this).operator*=(rhs);
    }

    Rect& Rect::operator/=(float rhs)
    {
        l /= rhs;
        t /= rhs;
        r /= rhs;
        b /= rhs;
        return *this;
    }

    Rect Rect::operator/(float rhs) const
    {
        return Rect(*this).operator/=(rhs);
    }

    bool Rect::operator==(const Rect& rhs) const
    {
        return (l == rhs.l && t == rhs.t && r == rhs.r && b == rhs.b);
    }

    bool Rect::operator!=(const Rect& rhs) const
    {
        return !operator==(rhs);
    }

    const bool Rect::PtInRect(const Point point) const
    {
        return (l <= point.x && t <= point.y && point.x < r && point.y < b);
    }

    const bool Rect::PtInRect(const float x, const float y) const
    {
        return (l <= x && t <= y && x < r && y < b);
    }

    const bool Rect::ContactTest(const Rect rect) const
    {
        return l < rect.r && t < rect.b && rect.l < r && rect.t < b;
    }

    Rect Rect::Inflate(const float x, const float y) const
    {
        return Rect(*this).operator+=(Point(x, y));
    }

    Rect Rect::Deflate(const float x, const float y) const
    {
        return Rect(*this).operator-=(Point(x, y));
    }
}
