#include <blik.hpp>
#include "blik_parser.hpp"

namespace BLIK
{
    Parser::Tag::Tag(const String key) : m_key_base(key), m_key(m_key_base)
    {
        BLIK_ASSERT("검색키의 길이가 0입니다", 0 < m_key_base.Length());
        Memory::Set(m_map, 0, sizeof(sint32) * 256);

        sint32 Focus = 0;
        while(m_key[++Focus])
            m_map[(sint32) m_key[Focus - 1]] = Focus;

        for(sint32 i = 0; i < 256; ++i)
            m_map[i] = Focus - m_map[i];
    }

    Parser::Tag::~Tag()
    {
    }

    Parser::Parser(const String source) : m_source_base(source), m_source(m_source_base)
    {
        m_offset = 0;
    }

    Parser::~Parser()
    {
    }

    const bool Parser::SeekOf(const Tag& tag)
    {
        const sint32 KeyLen = tag.m_map[0];
        const char KeyTail = tag.m_key[KeyLen - 1];
        for(sint32 i = m_offset + KeyLen - 1, iend = m_source_base.Length();
            i < iend; i += tag.m_map[m_source[i] & 0xFF])
        {
            if(m_source[i] != KeyTail) continue;
            if(!tag.m_key_base.Compare(m_source, i + 1 - KeyLen))
            {
                m_offset = i + 1;
                return true;
            }
        }
        return false;
    }

    const sint32 Parser::ReadInt()
    {
        return GetInt(m_source, m_source_base.Length(), &m_offset);
    }

    const sint32s Parser::ReadInts(const char separator, const char endmark)
    {
        sint32s Collector;
        do
        {
            Collector.AtAdding() = ReadInt();
            char OneChar = m_source[m_offset];
            while(OneChar == separator
                || OneChar == ' ' || OneChar == '\t'
                || OneChar == '\r' || OneChar == '\n')
                OneChar = m_source[++m_offset];
        }
        while(m_source[m_offset] != endmark && m_source[m_offset] != '\0');
        return Collector;
    }

    const float Parser::ReadFloat()
    {
        return GetFloat(m_source, m_source_base.Length(), &m_offset);
    }

    const floats Parser::ReadFloats(const char separator, const char endmark)
    {
        floats Collector;
        do
        {
            Collector.AtAdding() = ReadFloat();
            char OneChar = m_source[m_offset];
            while(OneChar == separator
                || OneChar == ' ' || OneChar == '\t'
                || OneChar == '\r' || OneChar == '\n')
                OneChar = m_source[++m_offset];
        }
        while(m_source[m_offset] != endmark && m_source[m_offset] != '\0');
        return Collector;
    }

    const uint32 Parser::ReadHex32()
    {
        if(m_source[m_offset] == '0'
            && (m_source[m_offset + 1] == 'x' || m_source[m_offset + 1] == 'X'))
            m_offset += 2;

        uint32 Value = 0;
        bool IsNumeric = false, IsHexLower = false, IsHexUpper = false;
        char OneChar = m_source[m_offset];
        while((IsNumeric = ('0' <= OneChar && OneChar <= '9'))
            || (IsHexLower = ('a' <= OneChar && OneChar <= 'f'))
            || (IsHexUpper = ('A' <= OneChar && OneChar <= 'F')))
        {
            if(IsNumeric) Value = (Value << 4) + OneChar - '0';
            else if(IsHexLower) Value = (Value << 4) + 10 + OneChar - 'a';
            else if(IsHexUpper) Value = (Value << 4) + 10 + OneChar - 'A';
            OneChar = m_source[++m_offset];
        }
        return Value;
    }
}
