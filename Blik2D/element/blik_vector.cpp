#include <blik.hpp>
#include "blik_vector.hpp"

namespace BLIK
{
    Vector::Vector()
    {
        x = 0;
        y = 0;
        vx = 0;
        vy = 0;
    }

    Vector::Vector(const Vector& rhs)
    {
        operator=(rhs);
    }

    Vector::Vector(float x, float y, float vx, float vy)
    {
        this->x = x;
        this->y = y;
        this->vx = vx;
        this->vy = vy;
    }

    Vector::Vector(const Point& pos, const Point& vec)
    {
        x = pos.x;
        y = pos.y;
        vx = vec.x;
        vy = vec.y;
    }

    Vector::~Vector()
    {
    }

    Vector& Vector::operator=(const Vector& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        vx = rhs.vx;
        vy = rhs.vy;
        return *this;
    }

    Vector& Vector::operator+=(const Point& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vector Vector::operator+(const Point& rhs) const
    {
        return Vector(*this).operator+=(rhs);
    }

    Vector& Vector::operator-=(const Point& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Vector Vector::operator-(const Point& rhs) const
    {
        return Vector(*this).operator-=(rhs);
    }

    bool Vector::operator==(const Vector& rhs) const
    {
        return (x == rhs.x && y == rhs.y && vx == rhs.vx && vy == rhs.vy);
    }

    bool Vector::operator!=(const Vector& rhs) const
    {
        return !operator==(rhs);
    }
}
