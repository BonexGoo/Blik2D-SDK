#include <blik.hpp>
#include "blik_wstring.hpp"

#include <stdarg.h>

namespace BLIK
{
    WString::WString() : m_words(BlankString())
    {
    }

    WString::WString(const WString& rhs) : m_words(rhs.m_words)
    {
    }

    WString::WString(const wchararray& rhs) : m_words(rhs)
    {
    }

    WString::WString(id_share_read rhs) : m_words(rhs)
    {
    }

    WString::WString(id_cloned_share_read rhs) : m_words(rhs)
    {
    }

    WString::WString(wchars rhs, sint32 length) : m_words(BlankString())
    {
        BLIK_ASSERT("정상적인 파라미터가 아닙니다", rhs && -1 <= length);
        if(*rhs != L'\0' && length != 0)
        {
            if(length == -1)
                operator=(rhs);
            else
            {
                m_words.AtWherever(length) = L'\0';
                Memory::Copy(m_words.AtDumping(0, length + 1), rhs,
                    sizeof(wchar_t) * length);
            }
        }
    }

    WString::WString(wchar_t rhs) : m_words(BlankString())
    {
        operator=(rhs);
    }

    WString::~WString()
    {
    }

    WString& WString::operator=(const WString& rhs)
    {
        m_words = rhs.m_words;
        return *this;
    }

    WString& WString::operator=(wchars rhs)
    {
        m_words.Clear();
        if(rhs)
        {
            do {m_words.AtAdding() = *rhs;}
            while(*(rhs++));
        }
        else m_words.AtAdding() = L'\0';
        return *this;
    }

    WString& WString::operator=(wchar_t rhs)
    {
        m_words.Clear();
        m_words.AtAdding() = rhs;
        m_words.AtAdding() = L'\0';
        return *this;
    }

    WString& WString::operator+=(const WString& rhs)
    {
        return operator+=((wchars) rhs);
    }

    WString& WString::operator+=(wchars rhs)
    {
        if(rhs)
        {
            m_words.SubtractionOne();
            do {m_words.AtAdding() = *rhs;}
            while(*(rhs++));
        }
        return *this;
    }

    WString& WString::operator+=(wchar_t rhs)
    {
        m_words.At(-1) = rhs;
        m_words.AtAdding() = L'\0';
        return *this;
    }

    WString WString::operator+(const WString& rhs) const
    {
        return WString(*this).operator+=(rhs);
    }

    WString WString::operator+(wchars rhs) const
    {
        return WString(*this).operator+=(rhs);
    }

    WString operator+(wchars lhs, const WString& rhs)
    {
        return WString(lhs).operator+=(rhs);
    }

    WString WString::operator+(wchar_t rhs) const
    {
        return WString(*this).operator+=(rhs);
    }

    WString operator+(wchar_t lhs, const WString& rhs)
    {
        return WString(lhs).operator+=(rhs);
    }

    bool WString::operator==(const WString& rhs) const
    {
        return (blik_wcscmp(&m_words[0], &rhs.m_words[0]) == 0);
    }

    bool WString::operator==(wchars rhs) const
    {
        return (blik_wcscmp(&m_words[0], rhs) == 0);
    }

    bool operator==(wchars lhs, const WString& rhs)
    {
        return (blik_wcscmp(lhs, &rhs.m_words[0]) == 0);
    }

    bool WString::operator!=(const WString& rhs) const
    {
        return !operator==(rhs);
    }

    bool WString::operator!=(wchars rhs) const
    {
        return !operator==(rhs);
    }

    bool operator!=(wchars lhs, const WString& rhs)
    {
        return !operator==(lhs, rhs);
    }

    WString::operator wchars() const
    {
        return &m_words[0];
    }

    WString::operator id_share() const
    {
        return m_words;
    }

    WString::operator id_cloned_share() const
    {
        return m_words;
    }

    void WString::Empty()
    {
        m_words = BlankString();
    }

    const wchar_t WString::operator[](sint32 index) const
    {
        return m_words[index];
    }

    sint32 WString::Length() const
    {
        return m_words.Count() - 1;
    }

    WString WString::Format(wchars text, ...)
    {
        va_list Args;
        va_start(Args, text);
        const sint32 Size = blik_vsnwprintf(nullptr, 0, text, Args);
        va_end(Args);

        WString Result;
        Result.m_words.AtWherever(Size) = L'\0';

        va_start(Args, text);
        blik_vsnwprintf(Result.m_words.AtDumping(0, Size + 1), Size + 1, text, Args);
        va_end(Args);
        return Result;
    }

    WString WString::FromChars(chars text, sint32 length)
    {
        wchararray NewWords;
        if(text)
        {
            chars textend = text + (length & 0x00FFFFFF);
            while((*text != '\0') & (text < textend))
            {
                if(sizeof(wchar_t) == 2)
                {
                    if((text[0] & 0xF8) == 0xF0) // 4Bytes
                    {
                        const char NewValue = (((text[0] & 0x07) << 2) | ((text[1] & 0x30) >> 4)) - 1;
                        NewWords.AtAdding() = 0xD800 | (NewValue << 6) | ((text[1] & 0x0F) << 2) | ((text[2] & 0x30) >> 4);
                        NewWords.AtAdding() = 0xDC00 | ((text[2] & 0x0F) << 6) | (text[3] & 0x3F);
                        text += 4;
                    }
                    else if((text[0] & 0xF0) == 0xE0) // 3Bytes
                    {
                        NewWords.AtAdding() = ((text[0] & 0x0F) << 12) | ((text[1] & 0x3F) << 6) | (text[2] & 0x3F);
                        text += 3;
                    }
                    else if((text[0] & 0xE0) == 0xC0) // 2Bytes
                    {
                        NewWords.AtAdding() = ((text[0] & 0x1F) << 6) | (text[1] & 0x3F);
                        text += 2;
                    }
                    else // 1Bytes
                    {
                        NewWords.AtAdding() = text[0] & 0xFF;
                        text += 1;
                    }
                }
                else if(sizeof(wchar_t) == 4)
                {
                    sint32 BytesToRead = 1;
                    const uint08 FirstByte = *text & 0xFF;
                    if(0xFC <= FirstByte) BytesToRead = 6;
                    else if(0xF8 <= FirstByte) BytesToRead = 5;
                    else if(0xF0 <= FirstByte) BytesToRead = 4;
                    else if(0xE0 <= FirstByte) BytesToRead = 3;
                    else if(0xC0 <= FirstByte) BytesToRead = 2;

                    wchar_t NewValue = 0;
                    switch(6 - BytesToRead)
                    {
                    case 0: NewValue += (*(text++) & 0xFF); NewValue <<= 6;
                    case 1: NewValue += (*(text++) & 0xFF); NewValue <<= 6;
                    case 2: NewValue += (*(text++) & 0xFF); NewValue <<= 6;
                    case 3: NewValue += (*(text++) & 0xFF); NewValue <<= 6;
                    case 4: NewValue += (*(text++) & 0xFF); NewValue <<= 6;
                    case 5: NewValue += (*(text++) & 0xFF);
                    }

                    static const wchar_t OffsetsFromUTF8[6] = {
                        (wchar_t) 0x00000000, (wchar_t) 0x00003080, (wchar_t) 0x000E2080,
                        (wchar_t) 0x03C82080, (wchar_t) 0xFA082080, (wchar_t) 0x82082080};
                    NewValue -= OffsetsFromUTF8[BytesToRead - 1];

                    if(0x0010FFFF < NewValue)
                        NewWords.AtAdding() = L'?';
                    else if(0x0000D800 <= NewValue && NewValue <= 0x0000DFFF)
                        NewWords.AtAdding() = L'?';
                    else NewWords.AtAdding() = NewValue;
                }
                else BLIK_ASSERT("지원되지 않는 wchar_t입니다", false);
            }
        }
        NewWords.AtAdding() = L'\0';
        return WString(NewWords);
    }

    sint32 WString::Compare(wchars text, wchars other, sint32 maxlength)
    {
		if(maxlength == -1)
			return blik_wcscmp(text, other);
		return blik_wcsncmp(text, other, maxlength);
    }

    sint32 WString::CompareNoCase(wchars text, wchars other, sint32 maxlength)
    {
        if(maxlength == -1)
			return blik_wcsicmp(text, other);
		return blik_wcsnicmp(text, other, maxlength);
    }

    const wchararray& WString::BlankString()
    {
        static const wchararray _(L'\0');
        return _;
    }
}
