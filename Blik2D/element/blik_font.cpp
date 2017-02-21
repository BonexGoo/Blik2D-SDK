#include <blik.hpp>
#include "blik_font.hpp"

namespace BLIK
{
    Font::Font()
    {
        name = "Arial";
        size = 10;
    }

    Font::Font(const Font& rhs)
    {
        operator=(rhs);
    }

    Font::Font(chars name, float size)
    {
        this->name = name;
        this->size = size;
    }

    Font::~Font()
    {
    }

    Font& Font::operator=(const Font& rhs)
    {
        name = rhs.name;
        size = rhs.size;
        return *this;
    }
}
