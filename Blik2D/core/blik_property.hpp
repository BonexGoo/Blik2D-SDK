#pragma once
#include <blik_map.hpp>
#include <blik_string.hpp>

namespace BLIK
{
    class Property;
    typedef Array<const Property*, datatype_pod_canmemcpy> CollectedProperties;
    enum CollectOption {CO_Null, CO_GetParent};
    enum ScriptType {ST_Json, ST_Xml};
    enum ScriptOption {SO_OnlyReference, SO_NeedCopy};

    //! \brief 트리형 프로퍼티지원
    class Property
    {
    public:
        /*!
        \brief 자식프로퍼티를 생성하며 접근(키워드식)
        \param key : 자식프로퍼티의 이름
        \param length : 이름의 길이(-1이면 자동설정)
        \return 해당 자식프로퍼티의 인스턴스
        */
        inline Property& At(chars key, sint32 length = -1)
        {return m_namableChild(key, length);}

        /*!
        \brief 자식프로퍼티를 생성하며 접근(배열식)
        \param index : 자식프로퍼티의 순번
        \return 해당 자식프로퍼티의 인스턴스
        */
        inline Property& At(sint32 index)
        {return m_indexableChild.AtWherever(index);}

        /*!
        \brief 자신의 값 설정
        \param value : 문자열값
        \param length : 길이(-1은 자동측정)
        */
        void Set(chars value, sint32 length = -1);

        /*!
        \brief 초기화
        */
        void Clear();

        /*!
        \brief Bin바이너리를 로드
        \param src : Bin바이너리
        \return 성공여부
        */
        bool LoadBin(bytes src);

        /*!
        \brief Bin바이너리로 세이브
        \param dst : 결과배열(기존배열에 추가할 경우)
        \return Bin바이너리
        */
        uint08s SaveBin(uint08s dst = uint08s::Null()) const;

        /*!
        \brief Json소스를 파싱하여 로드(buffer)
        \param src : Json소스
        \return 성공여부
        */
        bool LoadJson(buffer src);

        /*!
        \brief Json소스를 파싱하여 로드(chars)
        \param option : 복사필요여부(src가 곧 지워질 예정이라면 복사필요)
        \param src : Json소스
        \param length : src의 길이(-1이면 자동설정)
        \return 성공여부
        */
        bool LoadJson(ScriptOption option, chars src, sint32 length = -1);

        /*!
        \brief Json소스로 세이브
        \param dst : 결과스트링(기존스트링에 추가할 경우)
        \return Json소스
        */
        String SaveJson(String dst = String()) const;

        /*!
        \brief Xml소스를 파싱하여 로드(buffer)
        \param src : Xml소스
        \return 성공여부
        */
        bool LoadXml(buffer src);

        /*!
        \brief Xml소스를 파싱하여 로드(chars)
        \param option : 복사필요여부(src가 곧 지워질 예정이라면 복사필요)
        \param src : Xml소스
        \param length : src의 길이(-1이면 자동설정)
        \return 성공여부
        */
        bool LoadXml(ScriptOption option, chars src, sint32 length = -1);

        /*!
        \brief Xml소스로 세이브
        \param dst : 결과스트링(기존스트링에 추가할 경우)
        \return Xml소스
        */
        String SaveXml(String dst = String()) const;

        /*!
        \brief 키-밸류 매칭에 의한 수집
        \param key : 자식프로퍼티의 이름
        \param value : 매칭할 문자열값
        \return 수집된 프로퍼티들
        */
        CollectedProperties CollectByMatch(chars key, chars value, CollectOption option = CO_Null) const;

    public:
        /*!
        \brief 자식프로퍼티로 접근(키워드식)
        \param key : 자식프로퍼티의 이름
        \return 해당 자식프로퍼티의 인스턴스
        */
        inline const Property& operator()(chars key) const
        {
            Property* CurChild = m_namableChild.Access(key);
            return (CurChild)? *CurChild : NullChild();
        }

        /*!
        \brief 자식프로퍼티로 접근(키워드식/순번방식)
        \param order : 자식프로퍼티의 순번
        \param getname : 자식프로퍼티의 이름얻기
        \return 해당 자식프로퍼티의 인스턴스
        */
        inline const Property& operator()(sint32 order, chararray* getname = nullptr) const
        {
            Property* CurChild = m_namableChild.AccessByOrder(order, getname);
            return (CurChild)? *CurChild : NullChild();
        }

        /*!
        \brief 자식프로퍼티로 접근(배열식)
        \param index : 자식프로퍼티의 순번
        \return 해당 자식프로퍼티의 인스턴스
        */
        inline const Property& operator[](sint32 index) const
        {
            if(0 <= index && index < m_indexableChild.Count())
                return m_indexableChild[index];
            return NullChild();
        }

        /*!
        \brief 자식프로퍼티의 수량얻기(키워드식)
        \return 수량값
        */
        inline sint32 LengthOfNamable() const
        {return m_namableChild.Count();}

        /*!
        \brief 자식프로퍼티의 수량얻기(배열식)
        \return 수량값
        */
        inline sint32 LengthOfIndexable() const
        {return m_indexableChild.Count();}

        /*!
        \brief 자신의 유효여부
        \return 유효여부
        */
        inline bool IsValid() const
        {return (this != &NullChild());}

        /*!
        \brief 자신의 문자열값 얻기
        \return 문자열값
        */
        inline chars GetString() const
        {
            if(!m_parsedString)
            {
                m_parsedString = Buffer::Alloc(BLIK_DBG m_valueLength + 1);
                Memory::Copy(m_parsedString, m_valueOffset, m_valueLength);
                ((char*) m_parsedString)[m_valueLength] = '\0';
            }
            return (chars) m_parsedString;
        }

        /*!
        \brief 빠르게 자신의 문자열값 얻기(chars_endless)
        \return 문자열값
        */
        inline chars_endless GetStringFast(sint32* length) const
        {
            BLIK_ASSERT("length인수가 반드시 필요합니다", length);
            *length = m_valueLength;
            return m_valueOffset;
        }

        /*!
        \brief 자신의 정수값 얻기
        \return 정수값
        */
        inline const sint32 GetInt() const
        {
            if(!m_parsedInt)
                m_parsedInt = new sint32(Parser::GetInt(m_valueOffset, m_valueLength));
            return *m_parsedInt;
        }

        /*!
        \brief 자신의 소수값 얻기
        \return 소수값
        */
        inline const float GetFloat() const
        {
            if(!m_parsedFloat)
                m_parsedFloat = new float(Parser::GetFloat(m_valueOffset, m_valueLength));
            return *m_parsedFloat;
        }

        /*!
        \brief 자신의 문자열값 얻기(디폴트처리)
        \param value : 디폴트값
        \return 문자열값
        */
        inline chars GetString(chars value) const
        {
            return (m_valueOffset)? GetString() : value;
        }

        /*!
        \brief 자신의 정수값 얻기(디폴트처리)
        \param value : 디폴트값
        \return 정수값
        */
        inline const sint32 GetInt(const sint32 value) const
        {
            return (m_valueOffset)? GetInt() : value;
        }

        /*!
        \brief 자신의 소수값 얻기(디폴트처리)
        \param value : 디폴트값
        \return 소수값
        */
        inline const float GetFloat(const float value) const
        {
            return (m_valueOffset)? GetFloat() : value;
        }

        /*!
        \brief 자식프로퍼티 현황보고
        */
        void DebugPrint() const;

    public:
        /*!
        \brief 생성자
        */
        Property();

        /*!
        \brief 복사생성자
        \param rhs : 복사할 인스턴스
        */
        Property(const Property& rhs);

        /*!
        \brief 생성자(Bin바이너리를 로드)
        \param src : Bin바이너리
        */
        Property(bytes src);

        /*!
        \brief 생성자(스크립트를 파싱)
        \param type : 스크립트종류
        \param src : 스크립트소스
        */
        Property(ScriptType type, buffer src);

        /*!
        \brief 생성자(스크립트를 파싱)
        \param type : 스크립트종류
        \param option : 복사필요여부(src가 곧 지워질 예정이라면 복사필요)
        \param src : 스크립트소스
        \param length : src의 길이(-1이면 자동설정)
        */
        Property(ScriptType type, ScriptOption option, chars src, sint32 length = -1);

        /*!
        \brief 소멸자
        */
        ~Property();

        /*!
        \brief 복사
        \param rhs : 복사할 인스턴스
        \return 자기 객체
        */
        Property& operator=(const Property& rhs);

    private:
        static inline const Property& NullChild()
        {static const Property _; return _;}

    private:
        void SetValue(chars value, sint32 length);
        static chars FindMark(chars value, const char mark);
        static chars SkipBlank(chars value, bool exclude_nullzero);
        static chars SkipBlankReverse(chars value);

    private:
        Property* InitSource(Property* anyparent);
        // Bin
        static const String& GetBinHeader();
        bytes LoadBinCore(bytes src);
        void SaveBinCore(uint08s& dst, const String& name = String()) const;
        // Json
        bool LoadJsonCore(chars src);
        void SaveJsonCore(sint32 tab, String name, String& dst, bool indexable, bool lastchild) const;
        static void SaveJsonCoreCB(const MapPath* path, const Property* data, payload param);
        // Xml
        bool LoadXmlCore(chars src);
        void SaveXmlCore(sint32 tab, String name, String& dst) const;
        static void SaveXmlCoreCB(const MapPath* path, const Property* data, payload param);
        // Collect
        void CollectCore(const Property* parent, chars key, chars value, const sint32 length, CollectedProperties* result, CollectOption option) const;
        static void CollectCoreCB(const MapPath* path, const Property* data, payload param);
        // Debug
        void DebugPrintCore(sint32 tab, String name, bool indexable) const;
        static void DebugPrintCoreCB(const MapPath* path, const Property* data, payload param);

    private:
        class StringSource
        {
        public:
            void InitString(buffer src)
            {
                BLIK_ASSERT("중복된 초기화입니다", !m_buffer && !m_string);
                BLIK_ASSERT("src인수는 1바이트 단위여야 합니다", Buffer::SizeOf(src) == 1);
                BLIK_ASSERT("src인수는 null문자로 끝나야 합니다", ((chars) src)[Buffer::CountOf(src) - 1] == '\0');
                m_buffer = src;
                m_length = Buffer::CountOf(src) - 1;
            }

            void InitString(ScriptOption option, chars src, sint32 length)
            {
                BLIK_ASSERT("중복된 초기화입니다", !m_buffer && !m_string);
                if(option == SO_NeedCopy)
                {
                    BLIK_ASSERT("SO_NeedCopy모드에서 length인수는 -1값이 될 수 없습니다", length != -1);
                    BLIK_ASSERT("SO_NeedCopy모드에서 src인수는 null문자로 끝나야 합니다", src[length] == '\0');
                    m_buffer = Buffer::Alloc(BLIK_DBG length + 1);
                    Memory::Copy(m_buffer, src, length + 1);
                }
                else m_string = src;
                m_length = length; // PSO_OnlyReference의 경우 -1값도 가능
            }

            inline chars GetString() const
            {
                return (chars) (((ublock) m_buffer) | ((ublock) m_string));
            }

            inline sint32 GetLength(bool recalc)
            {
                if(recalc && m_length == -1)
                    m_length = blik_strlen(GetString());
                return m_length;
            }

        public:
            inline StringSource* Last()
            {
                return (m_next)? m_next : this;
            }

            inline void CertifyLast()
            {
                if(!Last()->GetString()) return;
                StringSource* NewSource = new StringSource();
                NewSource->m_next = m_next;
                m_next = NewSource;
            }

        public:
            StringSource() {m_buffer = nullptr; m_string = nullptr; m_length = 0; m_next = nullptr;}
            ~StringSource() {Buffer::Free(m_buffer); delete m_next;}
            StringSource(const StringSource& rhs) {BLIK_ASSERT("사용금지", false);}
            StringSource& operator=(const StringSource& rhs) {BLIK_ASSERT("사용금지", false); return *this;}

        private:
            buffer m_buffer;
            chars m_string;
            sint32 m_length;
            StringSource* m_next;
        };

        // 원본연결
        Object<StringSource> m_source;

        // 자식연결
        Map<Property> m_namableChild;
        Array<Property, datatype_class_nomemcpy, 0> m_indexableChild;

        // 자기데이터
        chars m_valueOffset;
        sint32 m_valueLength;
        mutable buffer m_parsedString;
        mutable sint32* m_parsedInt;
        mutable float* m_parsedFloat;
    };
    typedef Array<Property> Properties;
}
