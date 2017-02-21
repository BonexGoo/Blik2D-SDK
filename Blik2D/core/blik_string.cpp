#include <blik.hpp>
#include "blik_string.hpp"

#include <stdarg.h>

namespace BLIK
{
    String::String() : m_words(BlankString()), m_findmap(sint32s::Null())
    {
    }

    String::String(const String& rhs) : m_words(rhs.m_words), m_findmap(rhs.m_findmap)
    {
    }

    String::String(const chararray& rhs) : m_words(rhs), m_findmap(sint32s::Null())
    {
    }

    String::String(id_share_read rhs) : m_words(rhs), m_findmap(sint32s::Null())
    {
    }

    String::String(id_cloned_share_read rhs) : m_words(rhs), m_findmap(sint32s::Null())
    {
    }

    String::String(chars rhs, sint32 length) : m_words(BlankString()), m_findmap(sint32s::Null())
    {
        BLIK_ASSERT("정상적인 파라미터가 아닙니다", rhs && -1 <= length);
        if(*rhs != '\0' && length != 0)
        {
            if(length == -1)
                operator=(rhs);
            else
            {
                m_words.AtWherever(length) = '\0';
                Memory::Copy(m_words.AtDumping(0, length + 1), rhs,
                    sizeof(char) * length);
            }
        }
    }

    String::String(char rhs) : m_words(BlankString()), m_findmap(sint32s::Null())
    {
        operator=(rhs);
    }

    String::~String()
    {
    }

    String& String::operator=(const String& rhs)
    {
        m_words = rhs.m_words;
        m_findmap = rhs.m_findmap;
        return *this;
    }

    String& String::operator=(chars rhs)
    {
        m_words.Clear();
        if(rhs)
        {
            do {m_words.AtAdding() = *rhs;}
            while(*(rhs++));
        }
        else m_words.AtAdding() = '\0';
        m_findmap.Clear();
        return *this;
    }

    String& String::operator=(char rhs)
    {
        m_words.Clear();
        m_words.AtAdding() = rhs;
        m_words.AtAdding() = '\0';
        m_findmap.Clear();
        return *this;
    }

    String& String::operator+=(const String& rhs)
    {
        return operator+=((chars) rhs);
    }

    String& String::operator+=(chars rhs)
    {
        if(rhs)
        {
            m_words.SubtractionOne();
            do {m_words.AtAdding() = *rhs;}
            while(*(rhs++));
            m_findmap.Clear();
        }
        return *this;
    }

    String& String::operator+=(char rhs)
    {
        m_words.At(-1) = rhs;
        m_words.AtAdding() = '\0';
        m_findmap.Clear();
        return *this;
    }

    String String::operator+(const String& rhs) const
    {
        return String(*this).operator+=(rhs);
    }

    String String::operator+(chars rhs) const
    {
        return String(*this).operator+=(rhs);
    }

    String operator+(chars lhs, const String& rhs)
    {
        return String(lhs).operator+=(rhs);
    }

    String String::operator+(char rhs) const
    {
        return String(*this).operator+=(rhs);
    }

    String operator+(char lhs, const String& rhs)
    {
        return String(lhs).operator+=(rhs);
    }

    bool String::operator==(const String& rhs) const
    {
        return (blik_strcmp(&m_words[0], &rhs.m_words[0]) == 0);
    }

    bool String::operator==(chars rhs) const
    {
        return (blik_strcmp(&m_words[0], rhs) == 0);
    }

    bool operator==(chars lhs, const String& rhs)
    {
        return (blik_strcmp(lhs, &rhs.m_words[0]) == 0);
    }

    bool String::operator!=(const String& rhs) const
    {
        return !operator==(rhs);
    }

    bool String::operator!=(chars rhs) const
    {
        return !operator==(rhs);
    }

    bool operator!=(chars lhs, const String& rhs)
    {
        return !operator==(lhs, rhs);
    }

    String::operator chars() const
    {
        return &m_words[0];
    }

    String::operator id_share() const
    {
        return m_words;
    }

    String::operator id_cloned_share() const
    {
        return m_words;
    }

    void String::Empty()
    {
        m_words = BlankString();
        m_findmap = sint32s::Null();
    }

    const char String::operator[](sint32 index) const
    {
        return m_words[index];
    }

    sint32 String::Length() const
    {
        return m_words.Count() - 1;
    }

    sint32 String::Compare(const String& other, sint32 offset) const
    {
        BLIK_ASSERT("other의 범위를 초과한 접근입니다", 0 <= offset && offset < other.m_words.Count());
        return blik_strncmp(&m_words[0], &other.m_words[offset], Math::Min(m_words.Count(), other.m_words.Count() - offset));
    }

    sint32 String::CompareNoCase(const String& other, sint32 offset) const
    {
        BLIK_ASSERT("other의 범위를 초과한 접근입니다", 0 <= offset && offset < other.m_words.Count());
        return blik_strnicmp(&m_words[0], &other.m_words[offset], Math::Min(m_words.Count(), other.m_words.Count() - offset));
    }

    sint32 String::Find(sint32 index, const String& key) const
    {
        if(const sint32 KeyLength = key.Length())
        {
            chars KeyString = &key.m_words[0];
            const char KeyTail = key.m_words[-2];
            const sint32* KeyMap = key.GetFindMap();
            chars FindBegin = &m_words[0];
            chars FindFocus = FindBegin + index + KeyLength - 1;
            chars FindEnd = FindBegin + Length();
            while(FindFocus < FindEnd)
            {
                if(*FindFocus == KeyTail && !Memory::Compare(&FindFocus[1 - KeyLength], KeyString, KeyLength))
                    return &FindFocus[1 - KeyLength] - FindBegin;
                FindFocus += KeyMap[*FindFocus & 0xFF];
            }
        }
        return -1;
    }

    String& String::Replace(char oldword, char newword)
    {
        for(sint32 i = 0, iend = m_words.Count() - 1; i < iend; ++i)
            if(m_words[i] == oldword) m_words.At(i) = newword;
        m_findmap.Clear();
        return *this;
    }

    String& String::Add(chars other, sint32 length)
    {
        if(length == -1)
            operator+=(other);
        else if(other)
        {
            m_words.SubtractionOne();
            const sint32 OldLength = m_words.Count();
            m_words.AtWherever(OldLength + length) = '\0';
            Memory::Copy(m_words.AtDumping(OldLength, length + 1), other,
                sizeof(char) * length);
            m_findmap.Clear();
        }
        return *this;
    }

    String& String::Sub(sint32 length)
    {
        if(0 < length)
        {
            m_words.SubtractionOne();
            while(0 < length--)
                m_words.SubtractionOne();
            m_words.AtAdding() = '\0';
            m_findmap.Clear();
        }
        return *this;
    }

    String String::Left(sint32 length) const
    {
        if(Length() < length) length = Length();
        return String(&m_words[0], length);
    }

    String String::Right(sint32 length) const
    {
        if(Length() < length) length = Length();
        return String(&m_words[-length - 1], length);
    }

    bool String::ToFile(chars filename) const
    {
        if(id_asset TextAsset = Asset::OpenForWrite(filename, true))
        {
            Asset::Write(TextAsset, (bytes) &m_words[0], Length());
            Asset::Close(TextAsset);
            return true;
        }
        return false;
    }

    String String::Format(chars text, ...)
    {
        va_list Args;
        va_start(Args, text);
        const sint32 Size = blik_vsnprintf(nullptr, 0, text, Args);
        va_end(Args);

        String Result;
        Result.m_words.AtWherever(Size) = '\0';

        va_start(Args, text);
        blik_vsnprintf(Result.m_words.AtDumping(0, Size + 1), Size + 1, text, Args);
        va_end(Args);
        return Result;
    }

    String String::FromFile(chars filename, id_assetpath_read assetpath)
    {
        id_asset_read TextAsset = Asset::OpenForRead(filename, assetpath);
        if(!TextAsset) return String();

        String Result;
        const sint32 TextSize = Asset::Size(TextAsset);
        Result.m_words.AtWherever(TextSize) = '\0';
        Asset::Read(TextAsset, (uint08*) Result.m_words.AtDumping(0, TextSize + 1), TextSize);
        Asset::Close(TextAsset);
        return Result;
    }

    String String::FromWChars(wchars text, sint32 length)
    {
        chararray NewWords;
        if(text)
        {
            wchars textend = text + (length & 0x00FFFFFF);
            while((*text != L'\0') & (text < textend))
            {
                if(sizeof(wchar_t) == 2)
                {
                    const uint16& OneWChar = ((const uint16*) text)[0];
                    const uint16& TwoWChar = ((const uint16*) text)[1];
                    if((OneWChar & 0xFF80) == 0x0000) // Ascii
                    {
                        NewWords.AtAdding() = OneWChar & 0x00FF;
                        text += 1;
                    }
                    else if((OneWChar & 0xF800) == 0x0000) // 2Bytes-Less
                    {
                        NewWords.AtAdding() = 0xC0 | ((OneWChar & 0x07C0) >> 6);
                        NewWords.AtAdding() = 0x80 | (OneWChar & 0x003F);
                        text += 1;
                    }
                    else if((OneWChar & 0xFC00) != 0xD800 || (TwoWChar & 0xFC00) != 0xDC00) // 2Bytes-Full
                    {
                        NewWords.AtAdding() = 0xE0 | ((OneWChar & 0xF000) >> 12);
                        NewWords.AtAdding() = 0x80 | ((OneWChar & 0x0FC0) >> 6);
                        NewWords.AtAdding() = 0x80 | (OneWChar & 0x003F);
                        text += 1;
                    }
                    else // 4Bytes
                    {
                        const wchar_t NewValue = ((OneWChar & 0x03C0) >> 6) + 1;
                        NewWords.AtAdding() = 0xF0 | ((NewValue & 0x001C) >> 2);
                        NewWords.AtAdding() = 0x80 | ((NewValue & 0x0003) << 4) | ((OneWChar & 0x003C) >> 2);
                        NewWords.AtAdding() = 0x80 | ((OneWChar & 0x0003) << 4) | ((TwoWChar & 0x03C0) >> 6);
                        NewWords.AtAdding() = 0x80 | (TwoWChar & 0x003F);
                        text += 2;
                    }
                }
                else if(sizeof(wchar_t) == 4)
                {
                    const uint32& OneWChar = ((const uint32*) text)[0];
                    if(OneWChar < 0x80) // Ascii
                    {
                        NewWords.AtAdding() = 0x00 | ((OneWChar >>  0) & 0xFF);
                        text += 1;
                    }
                    else if(OneWChar < 0x800) // 2Bytes
                    {
                        NewWords.AtAdding() = 0xC0 | ((OneWChar >>  6) & 0xFF);
                        NewWords.AtAdding() = 0x80 | ((OneWChar >>  0) & 0x3F);
                        text += 1;
                    }
                    else if(OneWChar < 0x10000 || 0x0010FFFF < OneWChar) // 3Bytes
                    {
                        NewWords.AtAdding() = 0xE0 | ((OneWChar >> 12) & 0xFF);
                        NewWords.AtAdding() = 0x80 | ((OneWChar >>  6) & 0x3F);
                        NewWords.AtAdding() = 0x80 | ((OneWChar >>  0) & 0x3F);
                        text += 1;
                    }
                    else // 4Bytes
                    {
                        NewWords.AtAdding() = 0xF0 | ((OneWChar >> 18) & 0xFF);
                        NewWords.AtAdding() = 0x80 | ((OneWChar >> 12) & 0x3F);
                        NewWords.AtAdding() = 0x80 | ((OneWChar >>  6) & 0x3F);
                        NewWords.AtAdding() = 0x80 | ((OneWChar >>  0) & 0x3F);
                        text += 1;
                    }
                }
                else BLIK_ASSERT("지원되지 않는 wchar_t입니다", false);
            }
        }
        NewWords.AtAdding() = '\0';
        return String(NewWords);
    }

    String String::FromInteger(sint32 value)
    {
        char Result[1024];
        const sint32 ResultLength = blik_snprintf(Result, 1024, "%d", value);
        return String(Result, ResultLength);
    }

    String String::FromFloat(float value)
    {
        char Result[1024];
        const sint32 ResultLength = blik_snprintf(Result, 1024, "%f", value);
        return String(Result, ResultLength);
    }

    sint32 String::Compare(chars text, chars other, sint32 maxlength)
    {
        if(maxlength == -1)
            return blik_strcmp(text, other);
        return blik_strncmp(text, other, maxlength);
    }

    sint32 String::CompareNoCase(chars text, chars other, sint32 maxlength)
    {
        if(maxlength == -1)
            return blik_stricmp(text, other);
        return blik_strnicmp(text, other, maxlength);
    }

    const chararray& String::BlankString()
    {
        static const chararray _('\0');
        return _;
    }

    const sint32* String::GetFindMap() const
    {
        if(m_findmap.Count() == 0)
        {
            m_findmap.AtWherever(255);
            sint32* Findmap = m_findmap.AtDumping(0, 256);
            Memory::Set(Findmap, 0, sizeof(sint32) * 256);

            sint32 Focus = 0;
            chars String = &m_words[0];
            while(String[++Focus])
                Findmap[String[Focus - 1] & 0xFF] = Focus;
            for(sint32 i = 0; i < 256; ++i)
                Findmap[i] = Focus - Findmap[i];
        }
        BLIK_ASSERT("findmap의 규격이 잘못되었습니다", m_findmap.Count() == 256);
        return &m_findmap[0];
    }
}
