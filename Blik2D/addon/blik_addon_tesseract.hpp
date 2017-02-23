#pragma once
#include <addon/blik_addon.hpp>

namespace BLIK
{
    class TesseractClass
    {
    public:
        TesseractClass();
        TesseractClass(const TesseractClass& rhs);
        ~TesseractClass();
        TesseractClass& operator=(const TesseractClass& rhs);

    public:
        void Init(chars tifpath, chars boxpath);
        inline String GetTifPath() {return m_tifpath;}
        inline String GetOtherPath() {return m_otherpath;}

    private:
        String m_tifpath;
        String m_otherpath;
    };
}
