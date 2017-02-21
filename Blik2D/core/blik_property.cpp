#include <blik.hpp>
#include "blik_property.hpp"

namespace BLIK
{
    void Property::Set(chars value, sint32 length)
    {
        Buffer::Free(m_parsedString);
        delete m_parsedInt;
        delete m_parsedFloat;
        m_parsedString = nullptr;
        m_parsedInt = nullptr;
        m_parsedFloat = nullptr;

        sint32 Length = 0;
        if(value)
        {
            Length = (length == -1)? blik_strlen(value) : length;
            m_parsedString = Buffer::Alloc(BLIK_DBG Length + 1);
            Memory::Copy(m_parsedString, value, Length);
            ((char*) m_parsedString)[Length] = '\0';
        }
        m_valueOffset = (chars) m_parsedString;
        m_valueLength = Length;
    }

    void Property::Clear()
    {
        m_source.Clear();
        m_namableChild.Reset();
        m_indexableChild.Clear();
        Set(nullptr);
    }

    bool Property::LoadBin(bytes src)
    {
        if(GetBinHeader().Compare((chars) src))
            return false;

        m_namableChild.Reset();
        m_indexableChild.Clear();

        bytes SrcFocus = src + GetBinHeader().Length() + 1 + sizeof(sint32) + 1;
        LoadBinCore(SrcFocus);
        return true;
    }

    uint08s Property::SaveBin(uint08s dst) const
    {
        Memory::Copy(dst.AtDumping(dst.Count(), GetBinHeader().Length() + 1),
            (chars) GetBinHeader(), GetBinHeader().Length() + 1);
        SaveBinCore(dst);
        return dst;
    }

    bool Property::LoadJson(buffer src)
    {
        m_source.SharedValue().CertifyLast();
        StringSource* LastSource = m_source.SharedValue().Last();
        LastSource->InitString(src);
        return LoadJsonCore(LastSource->GetString());
    }

    bool Property::LoadJson(ScriptOption option, chars src, sint32 length)
    {
        m_source.SharedValue().CertifyLast();
        StringSource* LastSource = m_source.SharedValue().Last();
        LastSource->InitString(option, src, (option == SO_NeedCopy && length == -1)? blik_strlen(src) : length);
        return LoadJsonCore(LastSource->GetString());
    }

    String Property::SaveJson(String dst) const
    {
        const sint32 tab = 0;
        if(0 < m_namableChild.Count())
        {
            dst += "{\r\n";
            sint32 count = m_namableChild.Count();
            const void* param[3] = {&tab, &dst, &count};
            m_namableChild.AccessByCallback(SaveJsonCoreCB, param);
            dst += "}\r\n";
        }

        if(0 < m_indexableChild.Count())
        {
            dst += "[\r\n";
            for(sint32 i = 0, iend = m_indexableChild.Count(); i < iend; ++i)
                m_indexableChild[i].SaveJsonCore(tab + 1, String::FromInteger(i), dst, true, i + 1 == iend);
            dst += "]\r\n";
        }
        return dst;
    }

    bool Property::LoadXml(buffer src)
    {
        m_source.SharedValue().CertifyLast();
        StringSource* LastSource = m_source.SharedValue().Last();
        LastSource->InitString(src);
        return LoadXmlCore(LastSource->GetString());
    }

    bool Property::LoadXml(ScriptOption option, chars src, sint32 length)
    {
        m_source.SharedValue().CertifyLast();
        StringSource* LastSource = m_source.SharedValue().Last();
        LastSource->InitString(option, src, (option == SO_NeedCopy && length == -1)? blik_strlen(src) : length);
        return LoadXmlCore(LastSource->GetString());
    }

    String Property::SaveXml(String dst) const
    {
        for(sint32 i = 0, iend = m_indexableChild.Count(); i < iend; ++i)
            m_indexableChild[i].SaveXmlCore(0, String::FromInteger(i), dst);
        return dst;
    }

    CollectedProperties Property::CollectByMatch(chars key, chars value, CollectOption option) const
    {
        CollectedProperties Result;
        sint32 ValueLength = blik_strlen(value);
        CollectCore(nullptr, key, value, ValueLength, &Result, option);
        return Result;
    }

    void Property::DebugPrint() const
    {
        #if !BLIK_NDEBUG | BLIK_NEED_RELEASE_TRACE
            BLIK_TRACE("");
            BLIK_TRACE("==================== Property ====================");
            sint32 tab = -1;
            m_namableChild.AccessByCallback(DebugPrintCoreCB, &tab);
            for(sint32 i = 0, iend = m_indexableChild.Count(); i < iend; ++i)
                m_indexableChild[i].DebugPrintCore(0, String::FromInteger(i), true);
            BLIK_TRACE("==================================================");
        #endif
    }

    Property::Property() : m_indexableChild(Array<Property, datatype_class_nomemcpy, 0>::Null())
    {
        m_valueOffset = nullptr;
        m_valueLength = 0;
        m_parsedString = nullptr;
        m_parsedInt = nullptr;
        m_parsedFloat = nullptr;
    }

    Property::Property(const Property& rhs) :
        m_source(rhs.m_source),
        m_namableChild(rhs.m_namableChild),
        m_indexableChild(rhs.m_indexableChild)
    {
        m_valueOffset = rhs.m_valueOffset;
        m_valueLength = rhs.m_valueLength;
        m_parsedString = (rhs.m_parsedString)? Buffer::Clone(BLIK_DBG rhs.m_parsedString) : nullptr;
        m_parsedInt = (rhs.m_parsedInt)? new sint32(*rhs.m_parsedInt) : nullptr;
        m_parsedFloat = (rhs.m_parsedFloat)? new float(*rhs.m_parsedFloat) : nullptr;
        if(rhs.m_valueOffset == (chars) rhs.m_parsedString)
            m_valueOffset = (chars) m_parsedString;
    }

    Property::Property(bytes src) : m_indexableChild(Array<Property, datatype_class_nomemcpy, 0>::Null())
    {
        m_valueOffset = nullptr;
        m_valueLength = 0;
        m_parsedString = nullptr;
        m_parsedInt = nullptr;
        m_parsedFloat = nullptr;

        LoadBin(src);
    }

    Property::Property(ScriptType type, buffer src) : m_indexableChild(Array<Property, datatype_class_nomemcpy, 0>::Null())
    {
        m_valueOffset = nullptr;
        m_valueLength = 0;
        m_parsedString = nullptr;
        m_parsedInt = nullptr;
        m_parsedFloat = nullptr;

        switch(type)
        {
        case ST_Json: LoadJson(src); break;
        case ST_Xml: LoadXml(src); break;
        }
    }

    Property::Property(ScriptType type, ScriptOption option, chars src, sint32 length) : m_indexableChild(Array<Property, datatype_class_nomemcpy, 0>::Null())
    {
        m_valueOffset = nullptr;
        m_valueLength = 0;
        m_parsedString = nullptr;
        m_parsedInt = nullptr;
        m_parsedFloat = nullptr;

        switch(type)
        {
        case ST_Json: LoadJson(option, src, length); break;
        case ST_Xml: LoadXml(option, src, length); break;
        }
    }

    Property::~Property()
    {
        Buffer::Free(m_parsedString);
        delete m_parsedInt;
        delete m_parsedFloat;
    }

    Property& Property::operator=(const Property& rhs)
    {
        m_source = rhs.m_source;
        m_namableChild = rhs.m_namableChild;
        m_indexableChild = rhs.m_indexableChild;
        m_valueOffset = rhs.m_valueOffset;
        m_valueLength = rhs.m_valueLength;
        m_parsedString = (rhs.m_parsedString)? Buffer::Clone(BLIK_DBG rhs.m_parsedString) : nullptr;
        m_parsedInt = (rhs.m_parsedInt)? new sint32(*rhs.m_parsedInt) : nullptr;
        m_parsedFloat = (rhs.m_parsedFloat)? new float(*rhs.m_parsedFloat) : nullptr;
        if(rhs.m_valueOffset == (chars) rhs.m_parsedString)
            m_valueOffset = (chars) m_parsedString;
        return *this;
    }

    void Property::SetValue(chars value, sint32 length)
    {
        if(m_valueOffset)
        {
            Buffer::Free(m_parsedString);
            delete m_parsedInt;
            delete m_parsedFloat;
            m_parsedString = nullptr;
            m_parsedInt = nullptr;
            m_parsedFloat = nullptr;
        }

        m_valueOffset = value;
        m_valueLength = length;
    }

    chars Property::FindMark(chars value, const char mark)
    {
        chars V = value;
        while(*V && *V != mark) V++;
        return V;
    }

    chars Property::SkipBlank(chars value, bool exclude_nullzero)
    {
        chars V = value;
        while(*V && (*V == ' ' || *V == '\t' || *V == '\r' || *V == '\n')) V++;
        return V - (exclude_nullzero && *V == '\0');
    }

    chars Property::SkipBlankReverse(chars value)
    {
        chars V = value - 1;
        while(*V == ' ' || *V == '\t' || *V == '\r' || *V == '\n') V--;
        return V + 1;
    }

    Property* Property::InitSource(Property* anyparent)
    {
        m_source = anyparent->m_source;
        return this;
    }

    const String& Property::GetBinHeader()
    {static const String _ = String::Format("bin{%s %s}", __TIME__, __DATE__); return _;}

    bytes Property::LoadBinCore(bytes src)
    {
        // 데이터(원본)
        sint32 ValueLength = src[0];
        ValueLength |= (src[1] & 0xFF) << 8;
        ValueLength |= (src[2] & 0xFF) << 16;
        ValueLength |= (src[3] & 0xFF) << 24;
        src += sizeof(sint32);
        chars Value = (chars) src;
        src += ValueLength + 1;

        // 데이터(정수), 데이터(실수), 키워드식 자식수량, 배열식 자식수량
        sint32 DataField[4];
        Memory::Copy(DataField, src, sizeof(sint32) * 4);
        src += sizeof(sint32) * 4;

        // 셋팅
        Set(Value, ValueLength);
        m_parsedInt = new sint32(*((sint32*) &DataField[0]));
        m_parsedFloat = new float(*((float*) &DataField[1]));

        // 키워드식 자식
        for(sint32 i = 0; i < DataField[2]; ++i)
        {
            // 자식의 자기명칭
            sint32 NameLength = src[0];
            NameLength |= (src[1] & 0xFF) << 8;
            NameLength |= (src[2] & 0xFF) << 16;
            NameLength |= (src[3] & 0xFF) << 24;
            src += sizeof(sint32);
            chars Name = (chars) src;
            src += NameLength + 1;

            // 자식루프
            Property& NewChild = m_namableChild(Name);
            src = NewChild.LoadBinCore(src);
        }

        // 배열식 자식
        if(0 < DataField[3])
        {
            m_indexableChild.AtWherever(DataField[3] - 1);
            for(sint32 i = 0; i < DataField[3]; ++i)
            {
                // 자기명칭 스킵
                src += sizeof(sint32) + 1;

                // 자식루프
                Property& NewChild = m_indexableChild.At(i);
                src = NewChild.LoadBinCore(src);
            }
        }
        return src;
    }

    void Property::SaveBinCore(uint08s& dst, const String& name) const
    {
        // 자기명칭
        const sint32 NameLength = name.Length();
        Memory::Copy(dst.AtDumping(dst.Count(), sizeof(sint32)), &NameLength, sizeof(sint32));
        Memory::Copy(dst.AtDumping(dst.Count(), NameLength + 1), (chars) name, NameLength + 1);

        // 데이터(원본)
        Memory::Copy(dst.AtDumping(dst.Count(), sizeof(sint32)), &m_valueLength, sizeof(sint32));
        Memory::Copy(dst.AtDumping(dst.Count(), m_valueLength), m_valueOffset, m_valueLength);
        *dst.AtDumping(dst.Count(), 1) = (uint08) '\0';

        // 데이터(정수)
        const sint32 IntValue = GetInt();
        Memory::Copy(dst.AtDumping(dst.Count(), sizeof(sint32)), &IntValue, sizeof(sint32));
        // 데이터(실수)
        const float FloatValue = GetFloat();
        Memory::Copy(dst.AtDumping(dst.Count(), sizeof(float)), &FloatValue, sizeof(float));
        // 키워드식 자식수량
        const sint32 NChildCount = m_namableChild.Count();
        Memory::Copy(dst.AtDumping(dst.Count(), sizeof(sint32)), &NChildCount, sizeof(sint32));
        // 배열식 자식수량
        const sint32 IChildCount = m_indexableChild.Count();
        Memory::Copy(dst.AtDumping(dst.Count(), sizeof(sint32)), &IChildCount, sizeof(sint32));

        // 키워드식 자식
        for(sint32 i = 0; i < NChildCount; ++i)
        {
            chararray GetName;
            const Property* CurChild = m_namableChild.AccessByOrder(i, &GetName);
            CurChild->SaveBinCore(dst, GetName);
        }

        // 배열식 자식
        for(sint32 i = 0; i < IChildCount; ++i)
        {
            const Property& CurChild = m_indexableChild[i];
            CurChild.SaveBinCore(dst);
        }
    }

    bool Property::LoadJsonCore(chars src)
    {
		if(*src == '\0') return true;
        while(*src != '{' && *src != '[')
            if(*(++src) == '\0')
                return true;

        class ParseStack
        {
        public:
            ParseStack() : Object(nullptr), ChildIsIndexable(false) {}
            ~ParseStack() {}
        public:
            Property* Object;
            bool ChildIsIndexable;
        };
        Array<ParseStack> CurStack;
        CurStack.AtAdding().Object = nullptr;
        CurStack.AtAdding().Object = this;

        chars LastOffset = src;
        sint32 LastLength = 0;
        bool EtcMode = true;
        String EtcString;
        do switch(*src)
        {
        case ':':
            if(0 < LastLength)
            {
                Property* NewChild = nullptr;
                if(LastLength == 3 && LastOffset[0] == '~' && LastOffset[1] == '[' && LastOffset[2] == ']')
                    NewChild = CurStack[-1].Object->m_indexableChild.AtAdding().InitSource(this);
                else NewChild = CurStack[-1].Object->m_namableChild(LastOffset, LastLength).InitSource(this);
                CurStack.AtAdding().Object = NewChild;
                LastLength = 0;
            }
            else
            {
                BLIK_ASSERT("잘못된 Json스크립트입니다", false);
                return false;
            }
            break;

        case '{': case '[':
            {
                LastLength = 0;
                const char EndMark = (*src == '{')? '}' : ']';
                while(*(++src) == '\0' || *src == ' ' || *src == '\t' || *src == '\r' || *src == '\n');
                if(*src != EndMark)
                {
                    --src;
                    if(EndMark == ']')
                    {
                        CurStack.At(-1).ChildIsIndexable = true;
                        Property* NewChild = CurStack[-1].Object->m_indexableChild.AtAdding().InitSource(this);
                        CurStack.AtAdding().Object = NewChild;
                    }
                }
                else if(CurStack.Count() == 2)
                    return true;
            }
            break;

        case ',': case '}': case ']':
            if(CurStack.Count() <= 2)
            {
                BLIK_ASSERT("잘못된 Json스크립트입니다", false);
                return false;
            }
            if(0 < LastLength)
            {
                CurStack[-1].Object->SetValue(LastOffset, LastLength);
                LastLength = 0;
            }
            else if(String::Compare(EtcString, "null"))
            {
                CurStack[-1].Object->Set(EtcString, EtcString.Length());
                EtcString.Empty();
            }

            CurStack.SubtractionOne();
            if(*src == ',' && CurStack[-1].ChildIsIndexable)
            {
                Property* NewChild = CurStack[-1].Object->m_indexableChild.AtAdding().InitSource(this);
                CurStack.AtAdding().Object = NewChild;
            }
            else if((*src == '}' || *src == ']') && CurStack.Count() == 2)
                return true;
            EtcMode = false;
            break;

        case ' ': case '\t': case '\r': case '\n':
            EtcMode = false;
            break;

        case '\"': case '\'': // 스트링수집
            {
                LastOffset = src + 1;
                const char EndMark = *src;
                while(*(++src) && !(src[0] == EndMark && src[-1] != '\\'));
                LastLength = src - LastOffset;
                if(!*src) src--;
            }
            break;

        default:
            if(!EtcMode)
            {
                EtcMode = true;
                EtcString.Empty();
            }
            if(src[0] == '\\' && src[1] != '\0')
            {
                switch(*(++src))
                {
                case '\\': EtcString += '\\'; break;
                case 't': EtcString += '\t'; break;
                case 'r': EtcString += '\r'; break;
                case 'n': EtcString += '\n'; break;
                case '\"': EtcString += '\"'; break;
                case '\'': EtcString += '\''; break;
                }
            }
            else EtcString += *src;
            break;
        } while(*(++src) != '\0');

        BLIK_ASSERT("미완료된 Json스크립트입니다", false);
        return false;
    }

    void Property::SaveJsonCore(sint32 tab, String name, String& dst, bool indexable, bool lastchild) const
    {
        if(!indexable)
        {
            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            dst += '\"';
            dst += name;
            dst += "\":";
            if(m_valueOffset)
            {
                dst += '\"';
                dst.Add(m_valueOffset, m_valueLength);
                dst += (lastchild)? "\"" : "\",";
            }
            dst += "\r\n";
        }
        else if(m_valueOffset)
        {
            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            dst += '\"';
            dst.Add(m_valueOffset, m_valueLength);
            dst += (lastchild)? "\"" : "\",";
            dst += "\r\n";
        }

        const bool HasNamableChild = (0 < m_namableChild.Count());
        const bool HasIndexableChild = (0 < m_indexableChild.Count());

        if(HasNamableChild)
        {
            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            dst += "{\r\n";

            sint32 count = m_namableChild.Count();
            const void* param[3] = {&tab, &dst, &count};
            m_namableChild.AccessByCallback(SaveJsonCoreCB, param);

            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            dst += (lastchild && !HasIndexableChild)? "}\r\n" : "},\r\n";
        }

        if(HasIndexableChild)
        {
            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            dst += "[\r\n";

            for(sint32 i = 0, iend = m_indexableChild.Count(); i < iend; ++i)
                m_indexableChild[i].SaveJsonCore(tab + 1, String::FromInteger(i), dst, true, i + 1 == iend);

            for(sint32 i = 0; i < tab; ++i)
                dst += '\t';
            dst += (lastchild)? "]\r\n" : "],\r\n";
        }
    }

    void Property::SaveJsonCoreCB(const MapPath* path, const Property* data, payload param)
    {
        const sint32 tab = *((sint32*) ((void**) param)[0]);
        String& dst = *((String*) ((void**) param)[1]);
        sint32& count = *((sint32*) ((void**) param)[2]);
        data->SaveJsonCore(tab + 1, path->GetPath(), dst, false, --count == 0);
    }

    bool Property::LoadXmlCore(chars src)
    {
        while(*src != '<')
            if(*(++src) == '\0')
                return true;

        Array<Property*, datatype_pod_canmemcpy> CurStack;
        CurStack.AtAdding() = nullptr;
        CurStack.AtAdding() = this;

        auto StackPop = [this](Array<Property*, datatype_pod_canmemcpy>& stack, chars value, sint32 offset)->bool
        {
            if(stack.Count() <= 2) return false;
            else if(stack[-1]->LengthOfIndexable() == 0)
            {
                stack[-2]->m_namableChild.Remove("~tree");
                if(0 < offset) stack[-2]->At("~value").InitSource(this)->SetValue(value, offset);
            }
            stack.SubtractionOne();
            stack.SubtractionOne();
            return true;
        };

        auto AssertError = [](chars message)->bool
        {
            BLIK_ASSERT(message, false);
            return false;
        };

        chars LastSrc = src;
        bool ElementMode = false;
        do switch(*src)
        {
        case '<':
            if(!ElementMode)
            {
                if(src[+1] == '/')
                {
                    const Property* SavedProperty = (CurStack.Count() < 2)? nullptr : CurStack[-2];
                    if(!StackPop(CurStack, LastSrc + 1, src - (LastSrc + 1)))
                        return AssertError("엘리먼트를 팝하는 과정에서 잔여스택이 부족합니다");
                    chars NameBegin = src += 2;
                    src = FindMark(src, '>');

                    if(*src != '>') return AssertError("엘리먼트를 완료하는데 실패하였습니다");
                    else
                    {
                        sint32 SavedNameLength = 0;
                        chars_endless SavedName = (*SavedProperty)("@name").GetStringFast(&SavedNameLength);
                        if(src - NameBegin != SavedNameLength || Memory::Compare(NameBegin, SavedName, SavedNameLength))
                            return AssertError("엘리먼트를 팝하는 과정에서 네임매칭에 실패하였습니다");
                    }
                }
                else
                {
                    Property* NewChild = CurStack[-1]->m_indexableChild.AtAdding().InitSource(this);
                    if(src[+1] == '!' && src[+2] == '-' && src[+3] == '-')
                    {
                        NewChild->At("@name").InitSource(this)->SetValue(src + 1, 3);
                        chars CommentBegin = (src = SkipBlank(src + 4, false));
                        src = FindMark(src, '>');

                        if(*src == '>' && src[-1] == '-' && src[-2] == '-')
                        {
                            chars CommentEnd = SkipBlankReverse(src - 2);
                            NewChild->At("~value").InitSource(this)->SetValue(CommentBegin, CommentEnd - CommentBegin);
                        }
                        else return AssertError("주석엘리먼트를 완료하는데 실패하였습니다");
                    }
                    else
                    {
                        chars NameBegin = ++src;
                        while(*src && *src != '>' && *src != ' ' && *src != '\t' && *src != '\r' && *src != '\n')
                            src++;

                        const bool HasClosing = (*src == '>');
                        const bool IsChildless = (HasClosing && (src[-1] == '/' || src[-1] == '?'));
                        NewChild->At("@name").InitSource(this)->SetValue(NameBegin, src - NameBegin - IsChildless);

                        if(!HasClosing) ElementMode = true;
                        if(!IsChildless)
                        {
                            CurStack.AtAdding() = NewChild;
                            CurStack.AtAdding() = NewChild->At("~tree").InitSource(this);
                        }
                    }
                }
                LastSrc = src = SkipBlank(src, true);
            }
            else return AssertError("이전 엘리먼트가 완료되지 않고 <기호를 만났습니다");
            break;

        case '>':
            if(ElementMode)
            {
                ElementMode = false;
                if(src[-1] == '/' || src[-1] == '?')
                {
                    if(!StackPop(CurStack, nullptr, 0))
                        return AssertError("엘리먼트를 팝하는 과정에서 잔여스택이 부족합니다");
                }
                LastSrc = src;
            }
            break;

        case '=':
            if(ElementMode)
            {
                chars KeyEnd = SkipBlankReverse(src);
                Property* NewChild = CurStack[-2]->At(LastSrc, KeyEnd - LastSrc).InitSource(this);

                chars NameBegin = (src = SkipBlank(src + 1, false));
                const bool IsQuotes = (*NameBegin == '\"' || *NameBegin == '\'');
                if(IsQuotes)
                {
                    src++;
                    while(*src && *(src++) != *NameBegin);
                }
                else while(*src && *src != '>' && *src != '/' && *src != '?' && *src != ' ' && *src != '\t' && *src != '\r' && *src != '\n')
                    src++;
                NewChild->SetValue(NameBegin + IsQuotes, src - NameBegin - IsQuotes * 2);

                if(*src == '>') src--;
                else LastSrc = src = SkipBlank(src, true);
            }
            break;
        } while(*(++src) != '\0');

        if(ElementMode)
            return AssertError("이전 엘리먼트가 완료되지 않고 문서가 종료되었습니다");
        if(2 < CurStack.Count())
            return AssertError("엘리먼트의 잔여스택이 남은 상태에서 문서가 종료되었습니다");
        return true;
    }

    void Property::SaveXmlCore(sint32 tab, String name, String& dst) const
    {
        String TabString;
        for(sint32 i = 0; i < tab; ++i)
            TabString += '\t';
        dst += TabString;
        dst += '<';

        // 자기 엘리먼트(@name)
        String Name;
        if(Property* NameOption = m_namableChild.Access("@name"))
        {
            sint32 ValueLength = 0;
            chars_endless Value = NameOption->GetStringFast(&ValueLength);
            Name.Add(Value, ValueLength);
        }
        dst += Name;

        // 내부옵션들
        m_namableChild.AccessByCallback(SaveXmlCoreCB, &dst);

        const bool IsComment = (2 < Name.Length() && Name[0] == '!' && Name[1] == '-' && Name[2] == '-');
        bool HasChild = false;

        // 자기 스트링(~value)
        if(Property* ValueOption = m_namableChild.Access("~value"))
        {
            if(!IsComment) 
            {
                HasChild = true;
                dst += '>';
            }
            sint32 ValueLength = 0;
            chars_endless Value = ValueOption->GetStringFast(&ValueLength);
            dst.Add(Value, ValueLength);
        }
        // 자식 엘리먼트(~tree)
        if(Property* TreeOption = m_namableChild.Access("~tree"))
        {
            if(!HasChild)
            {
                HasChild = true;
                dst += ">\r\n";
            }
            for(sint32 i = 0, iend = TreeOption->m_indexableChild.Count(); i < iend; ++i)
                TreeOption->m_indexableChild[i].SaveXmlCore(tab + 1, String::FromInteger(i), dst);
            dst += TabString;
        }

        // 마감
        if(HasChild)
        {
            dst += "</";
            dst += Name;
            dst += ">\r\n";
        }
        else if(IsComment)
            dst += "-->\r\n";
        else if(0 < Name.Length() && Name[0] == '?')
            dst += "?>\r\n";
        else dst += "/>\r\n";
    }

    void Property::SaveXmlCoreCB(const MapPath* path, const Property* data, payload param)
    {
        String& dst = *((String*) param);
        const String Name = path->GetPath();
        if(String::Compare(Name, "@name") && String::Compare(Name, "~value") && String::Compare(Name, "~tree"))
        {
            dst += ' ';
            dst += Name;
            dst += "=\"";
            sint32 ValueLength = 0;
            chars_endless Value = data->GetStringFast(&ValueLength);
            dst.Add(Value, ValueLength);
            dst += '\"';
        }
    }

    void Property::CollectCore(const Property* parent, chars key, chars value, sint32 length, CollectedProperties* result, CollectOption option) const
    {
        if(const Property* CurChild = m_namableChild.Access(key))
        {
            sint32 GetLength = 0;
            chars_endless GetValue = CurChild->GetStringFast(&GetLength);
            if(GetLength == length && !Memory::Compare(GetValue, value, length))
            {
                if(option == CO_GetParent)
                    result->AtAdding() = this;
                else result->AtAdding() = (parent)? parent : &NullChild();
            }
        }

        const void* param[6] = {this, key, value, &length, result, &option};
        m_namableChild.AccessByCallback(CollectCoreCB, param);
        for(sint32 i = 0, iend = m_indexableChild.Count(); i < iend; ++i)
            m_indexableChild[i].CollectCore(this, key, value, length, result, option);
    }

    void Property::CollectCoreCB(const MapPath* path, const Property* data, payload param)
    {
        const Property* parent = (const Property*) ((void**) param)[0];
        chars key = (chars) ((void**) param)[1];
        chars value = (chars) ((void**) param)[2];
        sint32 length = *((sint32*) ((void**) param)[3]);
        CollectedProperties* result = (CollectedProperties*) ((void**) param)[4];
        CollectOption option = *((CollectOption*) ((void**) param)[5]);
        data->CollectCore(parent, key, value, length, result, option);
    }

    void Property::DebugPrintCore(sint32 tab, String name, bool indexable) const
    {
        #if !BLIK_NDEBUG | BLIK_NEED_RELEASE_TRACE
            char* TabString = new char[4 * tab + 1];
            Memory::Set(TabString, ' ', 4 * tab);
            TabString[4 * tab] = '\0';
            BLIK_TRACE(String::Format((indexable)? ">>>> %s[%s] " : ">>>> %s<%s> ", TabString, (chars) name)
                .Add(m_valueOffset, m_valueLength));
            delete[] TabString;

            m_namableChild.AccessByCallback(DebugPrintCoreCB, &tab);
            for(sint32 i = 0, iend = m_indexableChild.Count(); i < iend; ++i)
                m_indexableChild[i].DebugPrintCore(tab + 1, String::FromInteger(i), true);
        #endif
    }

    void Property::DebugPrintCoreCB(const MapPath* path, const Property* data, payload param)
    {data->DebugPrintCore(*((sint32*) param) + 1, path->GetPath(), false);}
}
