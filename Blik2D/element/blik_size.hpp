#pragma once
#include <blik.hpp>

namespace BLIK
{
    //! \brief 2D구간
    class Size
    {
    public:
        Size();
        Size(const Size& rhs);
        Size(float w, float h);
        ~Size();

        Size& operator=(const Size& rhs);
        bool operator==(const Size& rhs) const;
        bool operator!=(const Size& rhs) const;

    public:
        float w;
        float h;
    };
    typedef Array<Size, datatype_class_canmemcpy> Sizes;
}
