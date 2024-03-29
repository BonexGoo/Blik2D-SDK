﻿#include <blik.hpp>
#include "blik_wstring.hpp"

#include <stdarg.h>

extern wchars gWCharTable;
extern const uint16* gCp949Table;

static chars_cp949 Cp949Finder(wchar_t code)
{
	const sint32 Length = 98 + 51 + 2350;
    static wchar_t SortedWChars[Length];
    static uint16 SortedCp949s[Length];
    static bool NeedSort = true;
    if(NeedSort)
    {
        NeedSort = false;
        Memory::Copy(SortedWChars, gWCharTable, sizeof(wchar_t) * Length);
        Memory::Copy(SortedCp949s, gCp949Table, sizeof(uint16) * Length);
        for(sint32 i = 0; i < Length - 1; ++i)
		{
			sint32 iBest = i;
			for(sint32 j = i + 1; j < Length; ++j)
				if(SortedWChars[j] < SortedWChars[iBest])
					iBest = j;
			if(iBest != i)
			{
				const wchar_t TempA = SortedWChars[i];
				SortedWChars[i] = SortedWChars[iBest];
				SortedWChars[iBest] = TempA;
				const uint16 TempB = SortedCp949s[i];
				SortedCp949s[i] = SortedCp949s[iBest];
				SortedCp949s[iBest] = TempB;
			}
		}
    }

    sint32 Lower = 0, Upper = Length - 1;
	while(Lower <= Upper)
	{
		const sint32 Middle = (Lower + Upper) / 2;
		if(SortedWChars[Middle] == code)
			return (chars_cp949) &SortedCp949s[Middle];
		if(SortedWChars[Middle] < code)
			Lower = Middle + 1;
		else Upper = Middle - 1;
	}
	return (chars_cp949) "\xA1\xE0";
}

static wchars WCharFinder(char code1, char code2 = ' ')
{
    const sint32 Length = 98 + 51 + 2350;
    static wchar_t SortedWChars[Length];
    static uint16 SortedCp949s[Length];
    static bool NeedSort = true;
    if(NeedSort)
    {
        NeedSort = false;
        Memory::Copy(SortedWChars, gWCharTable, sizeof(wchar_t) * Length);
        Memory::Copy(SortedCp949s, gCp949Table, sizeof(uint16) * Length);
        for(sint32 i = 0; i < Length - 1; ++i)
		{
			sint32 iBest = i;
			for(sint32 j = i + 1; j < Length; ++j)
				if(SortedCp949s[j] < SortedCp949s[iBest])
					iBest = j;
			if(iBest != i)
			{
				const wchar_t TempA = SortedWChars[i];
				SortedWChars[i] = SortedWChars[iBest];
				SortedWChars[iBest] = TempA;
				const uint16 TempB = SortedCp949s[i];
				SortedCp949s[i] = SortedCp949s[iBest];
				SortedCp949s[iBest] = TempB;
			}
		}
    }

    const uint16 Code = ((code2 & 0xFF) << 8) | (code1 & 0xFF);
    sint32 Lower = 0, Upper = Length - 1;
	while(Lower <= Upper)
	{
		const sint32 Middle = (Lower + Upper) / 2;
		if(SortedCp949s[Middle] == Code)
			return &SortedWChars[Middle];
		if(SortedCp949s[Middle] < Code)
			Lower = Middle + 1;
		else Upper = Middle - 1;
	}
	return L"\x25A1";
}

namespace BLIK
{
    WString::WString() : m_words(BlankString())
    {
    }

    WString::WString(const WString& rhs) : m_words(rhs.m_words)
    {
    }

    WString::WString(WString&& rhs) : m_words(ToReference(rhs.m_words))
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

    WString& WString::operator=(WString&& rhs)
    {
        m_words = ToReference(rhs.m_words);
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

    buffer WString::MakeCp949(bool urlcode) const
    {
        auto AddUrlCode = [](chararray& dst, const char word)->void
        {
            const sint32 HiWord = ((word >> 4) & 0xF);
            const sint32 LoWord = (word & 0xF);
            dst.AtAdding() = '%';
            dst.AtAdding() = (HiWord < 10)? '0' + HiWord : 'A' + HiWord - 10;
            dst.AtAdding() = (LoWord < 10)? '0' + LoWord : 'A' + LoWord - 10;
        };

        chararray NewWords;
        wchars MyWords = (wchars) *this;
        const sint32 MyLength = Length();
        for(sint32 i = 0; MyWords[i] && i < MyLength; ++i)
        {
            chars_cp949 OneWord = Cp949Finder(MyWords[i]);
            if(OneWord[0] & 0x80)
            {
                if(urlcode)
                {
                    AddUrlCode(NewWords, OneWord[0]);
                    AddUrlCode(NewWords, OneWord[1]);
                }
                else
                {
                    NewWords.AtAdding() = OneWord[0];
                    NewWords.AtAdding() = OneWord[1];
                }
            }
            else if(urlcode && OneWord[0] == '%')
                AddUrlCode(NewWords, OneWord[0]);
            else NewWords.AtAdding() = OneWord[0];
        }
        NewWords.AtAdding() = '\0';
        return ((Share*)(id_share) NewWords)->CopiedBuffer();
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

    WString WString::FromCp949(bool urlcode, chars_cp949 text_cp949, sint32 length)
    {
        auto GetCode = [](chars_cp949 cp949)->char
        {
            sint32 HiWord = 0, LoWord = 0;
            if('0' <= cp949[0] && cp949[0] <= '9') HiWord = cp949[0] - '0';
            else if('A' <= cp949[0] && cp949[0] <= 'F') HiWord = cp949[0] - 'A' + 10;
            else if('a' <= cp949[0] && cp949[0] <= 'f') HiWord = cp949[0] - 'a' + 10;
            if('0' <= cp949[1] && cp949[1] <= '9') LoWord = cp949[1] - '0';
            else if('A' <= cp949[1] && cp949[1] <= 'F') LoWord = cp949[1] - 'A' + 10;
            else if('a' <= cp949[1] && cp949[1] <= 'f') LoWord = cp949[1] - 'a' + 10;
            return (char) ((HiWord << 4) | LoWord);
        };

        wchararray NewWords;
        for(sint32 i = 0, iend = (length < 0)? 0x7FFFFFFF : length; text_cp949[i] && i < iend; ++i)
        {
            if(text_cp949[i] & 0x80)
            {
                NewWords.AtAdding() = *WCharFinder(text_cp949[i], text_cp949[i + 1]);
                i++;
            }
            else if(urlcode && text_cp949[i] == '%')
            {
                const char OneWord = GetCode(&text_cp949[i + 1]);
                if((OneWord & 0x80) && text_cp949[i + 3] == '%')
                {
                    const char TwoWord = GetCode(&text_cp949[i + 4]);
                    NewWords.AtAdding() = *WCharFinder(OneWord, TwoWord);
                    i += 5;
                }
                else
                {
                    NewWords.AtAdding() = *WCharFinder(OneWord);
                    i += 2;
                }
            }
            else NewWords.AtAdding() = *WCharFinder(text_cp949[i]);
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

wchars gWCharTable =
	L"\t\n\r !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKL"
	L"MNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
	L"ㄱㄲㄳㄴㄵㄶㄷㄸㄹㄺㄻㄼㄽㄾㄿㅀㅁㅂㅃㅄㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ"
	L"ㅏㅐㅑㅒㅓㅔㅕㅖㅗㅘㅙㅚㅛㅜㅝㅞㅟㅠㅡㅢㅣ"
	L"가각간갇갈갉갊감갑값갓갔강갖갗같갚갛개객갠갤갬갭갯"
	L"갰갱갸갹갼걀걋걍걔걘걜거걱건걷걸걺검겁것겄겅겆겉겊"
	L"겋게겐겔겜겝겟겠겡겨격겪견겯결겸겹겻겼경곁계곈곌곕"
	L"곗고곡곤곧골곪곬곯곰곱곳공곶과곽관괄괆괌괍괏광괘괜"
	L"괠괨괬괭괴괵괸괼굄굅굇굉교굔굘굡굣구국군굳굴굵굶굻"
	L"굼굽굿궁궂궈궉권궐궜궝궤궷귀귁귄귈귐귑귓규균귤그극"
	L"근귿글긁금급긋긍긔기긱긴긷길긺김깁깃깅깆깊까깍깎깐"
	L"깔깖깜깝깟깠깡깥깨깩깬깰깸깹깻깼깽꺄꺅꺌꺼꺽꺾껀껄"
	L"껌껍껏껐껑께껙껜껨껫껭껴껸껼꼇꼈꼍꼐꼬꼭꼰꼲꼴꼼꼽"
	L"꼿꽁꽂꽃꽈꽉꽐꽜꽝꽤꽥꽹꾀꾄꾈꾐꾑꾕꾜꾸꾹꾼꿀꿇꿈"
	L"꿉꿋꿍꿎꿔꿜꿨꿩꿰꿱꿴꿸뀀뀁뀄뀌뀐뀔뀜뀝뀨끄끅끈끊"
	L"끌끎끓끔끕끗끙끝끼끽낀낄낌낍낏낑나낙낚난낟날낡낢남"
	L"납낫났낭낮낯낱낳내낵낸낼냄냅냇냈냉냐냑냔냘냠냥너넉"
	L"넋넌널넒넓넘넙넛넜넝넣네넥넨넬넴넵넷넸넹녀녁년녈념"
	L"녑녔녕녘녜녠노녹논놀놂놈놉놋농높놓놔놘놜놨뇌뇐뇔뇜"
	L"뇝뇟뇨뇩뇬뇰뇹뇻뇽누눅눈눋눌눔눕눗눙눠눴눼뉘뉜뉠뉨"
	L"뉩뉴뉵뉼늄늅늉느늑는늘늙늚늠늡늣능늦늪늬늰늴니닉닌"
	L"닐닖님닙닛닝닢다닥닦단닫달닭닮닯닳담답닷닸당닺닻닿"
	L"대댁댄댈댐댑댓댔댕댜더덕덖던덛덜덞덟덤덥덧덩덫덮데"
	L"덱덴델뎀뎁뎃뎄뎅뎌뎐뎔뎠뎡뎨뎬도독돈돋돌돎돐돔돕돗"
	L"동돛돝돠돤돨돼됐되된될됨됩됫됴두둑둔둘둠둡둣둥둬뒀"
	L"뒈뒝뒤뒨뒬뒵뒷뒹듀듄듈듐듕드득든듣들듦듬듭듯등듸디"
	L"딕딘딛딜딤딥딧딨딩딪따딱딴딸땀땁땃땄땅땋때땍땐땔땜"
	L"땝땟땠땡떠떡떤떨떪떫떰떱떳떴떵떻떼떽뗀뗄뗌뗍뗏뗐뗑"
	L"뗘뗬또똑똔똘똥똬똴뙈뙤뙨뚜뚝뚠뚤뚫뚬뚱뛔뛰뛴뛸뜀뜁"
	L"뜅뜨뜩뜬뜯뜰뜸뜹뜻띄띈띌띔띕띠띤띨띰띱띳띵라락란랄"
	L"람랍랏랐랑랒랖랗래랙랜랠램랩랫랬랭랴략랸럇량러럭런"
	L"럴럼럽럿렀렁렇레렉렌렐렘렙렛렝려력련렬렴렵렷렸령례"
	L"롄롑롓로록론롤롬롭롯롱롸롼뢍뢨뢰뢴뢸룀룁룃룅료룐룔"
	L"룝룟룡루룩룬룰룸룹룻룽뤄뤘뤠뤼뤽륀륄륌륏륑류륙륜률"
	L"륨륩륫륭르륵른를름릅릇릉릊릍릎리릭린릴림립릿링마막"
	L"만많맏말맑맒맘맙맛망맞맡맣매맥맨맬맴맵맷맸맹맺먀먁"
	L"먈먕머먹먼멀멂멈멉멋멍멎멓메멕멘멜멤멥멧멨멩며멱면"
	L"멸몃몄명몇몌모목몫몬몰몲몸몹못몽뫄뫈뫘뫙뫼묀묄묍묏"
	L"묑묘묜묠묩묫무묵묶문묻물묽묾뭄뭅뭇뭉뭍뭏뭐뭔뭘뭡뭣"
	L"뭬뮈뮌뮐뮤뮨뮬뮴뮷므믄믈믐믓미믹민믿밀밂밈밉밋밌밍"
	L"및밑바박밖밗반받발밝밞밟밤밥밧방밭배백밴밸뱀뱁뱃뱄"
	L"뱅뱉뱌뱍뱐뱝버벅번벋벌벎범법벗벙벚베벡벤벧벨벰벱벳"
	L"벴벵벼벽변별볍볏볐병볕볘볜보복볶본볼봄봅봇봉봐봔봤"
	L"봬뵀뵈뵉뵌뵐뵘뵙뵤뵨부북분붇불붉붊붐붑붓붕붙붚붜붤"
	L"붰붸뷔뷕뷘뷜뷩뷰뷴뷸븀븃븅브븍븐블븜븝븟비빅빈빌빎"
	L"빔빕빗빙빚빛빠빡빤빨빪빰빱빳빴빵빻빼빽뺀뺄뺌뺍뺏뺐"
	L"뺑뺘뺙뺨뻐뻑뻔뻗뻘뻠뻣뻤뻥뻬뼁뼈뼉뼘뼙뼛뼜뼝뽀뽁뽄"
	L"뽈뽐뽑뽕뾔뾰뿅뿌뿍뿐뿔뿜뿟뿡쀼쁑쁘쁜쁠쁨쁩삐삑삔삘"
	L"삠삡삣삥사삭삯산삳살삵삶삼삽삿샀상샅새색샌샐샘샙샛"
	L"샜생샤샥샨샬샴샵샷샹섀섄섈섐섕서석섞섟선섣설섦섧섬"
	L"섭섯섰성섶세섹센셀셈셉셋셌셍셔셕션셜셤셥셧셨셩셰셴"
	L"셸솅소속솎손솔솖솜솝솟송솥솨솩솬솰솽쇄쇈쇌쇔쇗쇘쇠"
	L"쇤쇨쇰쇱쇳쇼쇽숀숄숌숍숏숑수숙순숟술숨숩숫숭숯숱숲"
	L"숴쉈쉐쉑쉔쉘쉠쉥쉬쉭쉰쉴쉼쉽쉿슁슈슉슐슘슛슝스슥슨"
	L"슬슭슴습슷승시식신싣실싫심십싯싱싶싸싹싻싼쌀쌈쌉쌌"
	L"쌍쌓쌔쌕쌘쌜쌤쌥쌨쌩썅써썩썬썰썲썸썹썼썽쎄쎈쎌쎙쏘"
	L"쏙쏜쏟쏠쏢쏨쏩쏭쏴쏵쏸쐈쐐쐤쐬쐰쐴쐼쐽쑈쑤쑥쑨쑬쑴"
	L"쑵쑹쒀쒔쒜쒸쒼쓩쓰쓱쓴쓸쓺쓿씀씁씌씐씔씜씨씩씬씰씸"
	L"씹씻씽아악안앉않알앍앎앓암압앗았앙앝앞애액앤앨앰앱"
	L"앳앴앵야약얀얄얇얌얍얏양얕얗얘얜얠얩어억언얹얻얼얽"
	L"얾엄업없엇었엉엊엌엎에엑엔엘엠엡엣엥여역엮연열엶엷"
	L"염엽엾엿였영옅옆옇예옌옐옘옙옛옜오옥온올옭옮옰옳옴"
	L"옵옷옹옻와왁완왈왐왑왓왔왕왜왝왠왬왯왱외왹왼욀욈욉"
	L"욋욍요욕욘욜욤욥욧용우욱운울욹욺움웁웃웅워웍원월웜"
	L"웝웠웡웨웩웬웰웸웹웽위윅윈윌윔윕윗윙유육윤율윰윱윳"
	L"융윷으윽은을읊음읍읏응읒읓읔읕읖읗의읜읠읨읫이익인"
	L"일읽읾잃임입잇있잉잊잎자작잔잖잗잘잚잠잡잣잤장잦재"
	L"잭잰잴잼잽잿쟀쟁쟈쟉쟌쟎쟐쟘쟝쟤쟨쟬저적전절젊점접"
	L"젓정젖제젝젠젤젬젭젯젱져젼졀졈졉졌졍졔조족존졸졺좀"
	L"좁좃종좆좇좋좌좍좔좝좟좡좨좼좽죄죈죌죔죕죗죙죠죡죤"
	L"죵주죽준줄줅줆줌줍줏중줘줬줴쥐쥑쥔쥘쥠쥡쥣쥬쥰쥴쥼"
	L"즈즉즌즐즘즙즛증지직진짇질짊짐집짓징짖짙짚짜짝짠짢"
	L"짤짧짬짭짯짰짱째짹짼쨀쨈쨉쨋쨌쨍쨔쨘쨩쩌쩍쩐쩔쩜쩝"
	L"쩟쩠쩡쩨쩽쪄쪘쪼쪽쫀쫄쫌쫍쫏쫑쫓쫘쫙쫠쫬쫴쬈쬐쬔쬘"
	L"쬠쬡쭁쭈쭉쭌쭐쭘쭙쭝쭤쭸쭹쮜쮸쯔쯤쯧쯩찌찍찐찔찜찝"
	L"찡찢찧차착찬찮찰참찹찻찼창찾채책챈챌챔챕챗챘챙챠챤"
	L"챦챨챰챵처척천철첨첩첫첬청체첵첸첼쳄쳅쳇쳉쳐쳔쳤쳬"
	L"쳰촁초촉촌촐촘촙촛총촤촨촬촹최쵠쵤쵬쵭쵯쵱쵸춈추축"
	L"춘출춤춥춧충춰췄췌췐취췬췰췸췹췻췽츄츈츌츔츙츠측츤"
	L"츨츰츱츳층치칙친칟칠칡침칩칫칭카칵칸칼캄캅캇캉캐캑"
	L"캔캘캠캡캣캤캥캬캭컁커컥컨컫컬컴컵컷컸컹케켁켄켈켐"
	L"켑켓켕켜켠켤켬켭켯켰켱켸코콕콘콜콤콥콧콩콰콱콴콸쾀"
	L"쾅쾌쾡쾨쾰쿄쿠쿡쿤쿨쿰쿱쿳쿵쿼퀀퀄퀑퀘퀭퀴퀵퀸퀼큄"
	L"큅큇큉큐큔큘큠크큭큰클큼큽킁키킥킨킬킴킵킷킹타탁탄"
	L"탈탉탐탑탓탔탕태택탠탤탬탭탯탰탱탸턍터턱턴털턺텀텁"
	L"텃텄텅테텍텐텔템텝텟텡텨텬텼톄톈토톡톤톨톰톱톳통톺"
	L"톼퇀퇘퇴퇸툇툉툐투툭툰툴툼툽툿퉁퉈퉜퉤튀튁튄튈튐튑"
	L"튕튜튠튤튬튱트특튼튿틀틂틈틉틋틔틘틜틤틥티틱틴틸팀"
	L"팁팃팅파팍팎판팔팖팜팝팟팠팡팥패팩팬팰팸팹팻팼팽퍄"
	L"퍅퍼퍽펀펄펌펍펏펐펑페펙펜펠펨펩펫펭펴편펼폄폅폈평"
	L"폐폘폡폣포폭폰폴폼폽폿퐁퐈퐝푀푄표푠푤푭푯푸푹푼푿"
	L"풀풂품풉풋풍풔풩퓌퓐퓔퓜퓟퓨퓬퓰퓸퓻퓽프픈플픔픕픗"
	L"피픽핀필핌핍핏핑하학한할핥함합핫항해핵핸핼햄햅햇했"
	L"행햐향허헉헌헐헒험헙헛헝헤헥헨헬헴헵헷헹혀혁현혈혐"
	L"협혓혔형혜혠혤혭호혹혼홀홅홈홉홋홍홑화확환활홧황홰"
	L"홱홴횃횅회획횐횔횝횟횡효횬횰횹횻후훅훈훌훑훔훗훙훠"
	L"훤훨훰훵훼훽휀휄휑휘휙휜휠휨휩휫휭휴휵휸휼흄흇흉흐"
	L"흑흔흖흗흘흙흠흡흣흥흩희흰흴흼흽힁히힉힌힐힘힙힛힝";
const uint16* gCp949Table = (const uint16*)
    "\t \n \r   ! \" # $ % & \' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G H I J K L "
	"M N O P Q R S T U V W X Y Z [ \\ ] ^ _ ` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~ "
    "\xA4\xA1\xA4\xA2\xA4\xA3\xA4\xA4\xA4\xA5\xA4\xA6\xA4\xA7\xA4\xA8\xA4\xA9\xA4\xAA\xA4\xAB\xA4\xAC\xA4\xAD\xA4\xAE\xA4\xAF\xA4\xB0\xA4\xB1\xA4\xB2\xA4\xB3\xA4\xB4\xA4\xB5\xA4\xB6\xA4\xB7\xA4\xB8\xA4\xB9\xA4\xBA\xA4\xBB\xA4\xBC\xA4\xBD\xA4\xBE"
    "\xA4\xBF\xA4\xC0\xA4\xC1\xA4\xC2\xA4\xC3\xA4\xC4\xA4\xC5\xA4\xC6\xA4\xC7\xA4\xC8\xA4\xC9\xA4\xCA\xA4\xCB\xA4\xCC\xA4\xCD\xA4\xCE\xA4\xCF\xA4\xD0\xA4\xD1\xA4\xD2\xA4\xD3"
    "\xB0\xA1\xB0\xA2\xB0\xA3\xB0\xA4\xB0\xA5\xB0\xA6\xB0\xA7\xB0\xA8\xB0\xA9\xB0\xAA\xB0\xAB\xB0\xAC\xB0\xAD\xB0\xAE\xB0\xAF\xB0\xB0\xB0\xB1\xB0\xB2\xB0\xB3\xB0\xB4\xB0\xB5\xB0\xB6\xB0\xB7\xB0\xB8\xB0\xB9"
    "\xB0\xBA\xB0\xBB\xB0\xBC\xB0\xBD\xB0\xBE\xB0\xBF\xB0\xC0\xB0\xC1\xB0\xC2\xB0\xC3\xB0\xC4\xB0\xC5\xB0\xC6\xB0\xC7\xB0\xC8\xB0\xC9\xB0\xCA\xB0\xCB\xB0\xCC\xB0\xCD\xB0\xCE\xB0\xCF\xB0\xD0\xB0\xD1\xB0\xD2"
    "\xB0\xD3\xB0\xD4\xB0\xD5\xB0\xD6\xB0\xD7\xB0\xD8\xB0\xD9\xB0\xDA\xB0\xDB\xB0\xDC\xB0\xDD\xB0\xDE\xB0\xDF\xB0\xE0\xB0\xE1\xB0\xE2\xB0\xE3\xB0\xE4\xB0\xE5\xB0\xE6\xB0\xE7\xB0\xE8\xB0\xE9\xB0\xEA\xB0\xEB"
    "\xB0\xEC\xB0\xED\xB0\xEE\xB0\xEF\xB0\xF0\xB0\xF1\xB0\xF2\xB0\xF3\xB0\xF4\xB0\xF5\xB0\xF6\xB0\xF7\xB0\xF8\xB0\xF9\xB0\xFA\xB0\xFB\xB0\xFC\xB0\xFD\xB0\xFE\xB1\xA1\xB1\xA2\xB1\xA3\xB1\xA4\xB1\xA5\xB1\xA6"
    "\xB1\xA7\x82\x52\xB1\xA9\xB1\xAA\xB1\xAB\xB1\xAC\xB1\xAD\xB1\xAE\xB1\xAF\xB1\xB0\xB1\xB1\xB1\xB2\xB1\xB3\xB1\xB4\xB1\xB5\xB1\xB6\xB1\xB7\xB1\xB8\xB1\xB9\xB1\xBA\xB1\xBB\xB1\xBC\xB1\xBD\xB1\xBE\xB1\xBF"
    "\xB1\xC0\xB1\xC1\xB1\xC2\xB1\xC3\xB1\xC4\xB1\xC5\xB1\xC6\xB1\xC7\xB1\xC8\xB1\xC9\xB1\xCA\xB1\xCB\xB1\xCC\xB1\xCD\xB1\xCE\xB1\xCF\xB1\xD0\xB1\xD1\xB1\xD2\xB1\xD3\xB1\xD4\xB1\xD5\xB1\xD6\xB1\xD7\xB1\xD8"
    "\xB1\xD9\xB1\xDA\xB1\xDB\xB1\xDC\xB1\xDD\xB1\xDE\xB1\xDF\xB1\xE0\xB1\xE1\xB1\xE2\xB1\xE3\xB1\xE4\xB1\xE5\xB1\xE6\xB1\xE7\xB1\xE8\xB1\xE9\xB1\xEA\xB1\xEB\xB1\xEC\xB1\xED\xB1\xEE\xB1\xEF\xB1\xF0\xB1\xF1"
    "\xB1\xF2\xB1\xF3\xB1\xF4\xB1\xF5\xB1\xF6\xB1\xF7\xB1\xF8\xB1\xF9\xB1\xFA\xB1\xFB\xB1\xFC\xB1\xFD\xB1\xFE\xB2\xA1\xB2\xA2\xB2\xA3\xB2\xA4\xB2\xA5\xB2\xA6\xB2\xA7\xB2\xA8\xB2\xA9\xB2\xAA\xB2\xAB\xB2\xAC"
    "\xB2\xAD\xB2\xAE\xB2\xAF\xB2\xB0\xB2\xB1\xB2\xB2\xB2\xB3\xB2\xB4\xB2\xB5\xB2\xB6\xB2\xB7\xB2\xB8\xB2\xB9\xB2\xBA\xB2\xBB\xB2\xBC\xB2\xBD\xB2\xBE\xB2\xBF\xB2\xC0\xB2\xC1\xB2\xC2\xB2\xC3\xB2\xC4\xB2\xC5"
    "\xB2\xC6\xB2\xC7\xB2\xC8\xB2\xC9\xB2\xCA\xB2\xCB\xB2\xCC\xB2\xCD\xB2\xCE\xB2\xCF\xB2\xD0\xB2\xD1\xB2\xD2\xB2\xD3\xB2\xD4\xB2\xD5\xB2\xD6\xB2\xD7\xB2\xD8\xB2\xD9\xB2\xDA\xB2\xDB\xB2\xDC\xB2\xDD\xB2\xDE"
    "\xB2\xDF\xB2\xE0\xB2\xE1\xB2\xE2\xB2\xE3\xB2\xE4\xB2\xE5\xB2\xE6\xB2\xE7\xB2\xE8\xB2\xE9\xB2\xEA\xB2\xEB\xB2\xEC\xB2\xED\xB2\xEE\xB2\xEF\xB2\xF0\xB2\xF1\xB2\xF2\xB2\xF3\xB2\xF4\xB2\xF5\xB2\xF6\xB2\xF7"
    "\xB2\xF8\xB2\xF9\xB2\xFA\xB2\xFB\xB2\xFC\xB2\xFD\xB2\xFE\xB3\xA1\xB3\xA2\xB3\xA3\xB3\xA4\xB3\xA5\xB3\xA6\xB3\xA7\xB3\xA8\xB3\xA9\xB3\xAA\xB3\xAB\xB3\xAC\xB3\xAD\xB3\xAE\xB3\xAF\xB3\xB0\xB3\xB1\xB3\xB2"
    "\xB3\xB3\xB3\xB4\xB3\xB5\xB3\xB6\xB3\xB7\xB3\xB8\xB3\xB9\xB3\xBA\xB3\xBB\xB3\xBC\xB3\xBD\xB3\xBE\xB3\xBF\xB3\xC0\xB3\xC1\xB3\xC2\xB3\xC3\xB3\xC4\xB3\xC5\xB3\xC6\xB3\xC7\xB3\xC8\xB3\xC9\xB3\xCA\xB3\xCB"
    "\xB3\xCC\xB3\xCD\xB3\xCE\xB3\xCF\xB3\xD0\xB3\xD1\xB3\xD2\xB3\xD3\xB3\xD4\xB3\xD5\xB3\xD6\xB3\xD7\xB3\xD8\xB3\xD9\xB3\xDA\xB3\xDB\xB3\xDC\xB3\xDD\xB3\xDE\xB3\xDF\xB3\xE0\xB3\xE1\xB3\xE2\xB3\xE3\xB3\xE4"
    "\xB3\xE5\xB3\xE6\xB3\xE7\xB3\xE8\xB3\xE9\xB3\xEA\xB3\xEB\xB3\xEC\xB3\xED\xB3\xEE\xB3\xEF\xB3\xF0\xB3\xF1\xB3\xF2\xB3\xF3\xB3\xF4\xB3\xF5\xB3\xF6\xB3\xF7\xB3\xF8\xB3\xF9\xB3\xFA\xB3\xFB\xB3\xFC\xB3\xFD"
    "\xB3\xFE\xB4\xA1\xB4\xA2\xB4\xA3\xB4\xA4\xB4\xA5\xB4\xA6\xB4\xA7\xB4\xA8\xB4\xA9\xB4\xAA\xB4\xAB\xB4\xAC\xB4\xAD\xB4\xAE\xB4\xAF\xB4\xB0\xB4\xB1\xB4\xB2\xB4\xB3\xB4\xB4\xB4\xB5\xB4\xB6\xB4\xB7\xB4\xB8"
    "\xB4\xB9\xB4\xBA\xB4\xBB\xB4\xBC\xB4\xBD\xB4\xBE\xB4\xBF\xB4\xC0\xB4\xC1\xB4\xC2\xB4\xC3\xB4\xC4\xB4\xC5\xB4\xC6\xB4\xC7\xB4\xC8\xB4\xC9\xB4\xCA\xB4\xCB\xB4\xCC\xB4\xCD\xB4\xCE\xB4\xCF\xB4\xD0\xB4\xD1"
    "\xB4\xD2\x88\x9A\xB4\xD4\xB4\xD5\xB4\xD6\xB4\xD7\xB4\xD8\xB4\xD9\xB4\xDA\xB4\xDB\xB4\xDC\xB4\xDD\xB4\xDE\xB4\xDF\xB4\xE0\xB4\xE1\xB4\xE2\xB4\xE3\xB4\xE4\xB4\xE5\xB4\xE6\xB4\xE7\xB4\xE8\xB4\xE9\xB4\xEA"
    "\xB4\xEB\xB4\xEC\xB4\xED\xB4\xEE\xB4\xEF\xB4\xF0\xB4\xF1\xB4\xF2\xB4\xF3\xB4\xF4\xB4\xF5\xB4\xF6\xB4\xF7\xB4\xF8\xB4\xF9\xB4\xFA\xB4\xFB\xB4\xFC\xB4\xFD\xB4\xFE\xB5\xA1\xB5\xA2\xB5\xA3\xB5\xA4\xB5\xA5"
    "\xB5\xA6\xB5\xA7\xB5\xA8\xB5\xA9\xB5\xAA\xB5\xAB\xB5\xAC\xB5\xAD\xB5\xAE\xB5\xAF\xB5\xB0\xB5\xB1\xB5\xB2\xB5\xB3\xB5\xB4\xB5\xB5\xB5\xB6\xB5\xB7\xB5\xB8\xB5\xB9\xB5\xBA\xB5\xBB\xB5\xBC\xB5\xBD\xB5\xBE"
    "\xB5\xBF\xB5\xC0\xB5\xC1\xB5\xC2\xB5\xC3\xB5\xC4\xB5\xC5\xB5\xC6\xB5\xC7\xB5\xC8\xB5\xC9\xB5\xCA\xB5\xCB\xB5\xCC\xB5\xCD\xB5\xCE\xB5\xCF\xB5\xD0\xB5\xD1\xB5\xD2\xB5\xD3\xB5\xD4\xB5\xD5\xB5\xD6\xB5\xD7"
    "\xB5\xD8\xB5\xD9\xB5\xDA\xB5\xDB\xB5\xDC\xB5\xDD\xB5\xDE\xB5\xDF\xB5\xE0\xB5\xE1\xB5\xE2\xB5\xE3\xB5\xE4\xB5\xE5\xB5\xE6\xB5\xE7\xB5\xE8\xB5\xE9\xB5\xEA\xB5\xEB\xB5\xEC\xB5\xED\xB5\xEE\xB5\xEF\xB5\xF0"
    "\xB5\xF1\xB5\xF2\xB5\xF3\xB5\xF4\xB5\xF5\xB5\xF6\xB5\xF7\xB5\xF8\xB5\xF9\xB5\xFA\xB5\xFB\xB5\xFC\xB5\xFD\xB5\xFE\xB6\xA1\xB6\xA2\xB6\xA3\xB6\xA4\xB6\xA5\xB6\xA6\xB6\xA7\xB6\xA8\xB6\xA9\xB6\xAA\xB6\xAB"
    "\xB6\xAC\xB6\xAD\xB6\xAE\xB6\xAF\xB6\xB0\xB6\xB1\xB6\xB2\xB6\xB3\xB6\xB4\xB6\xB5\xB6\xB6\xB6\xB7\xB6\xB8\xB6\xB9\xB6\xBA\xB6\xBB\xB6\xBC\xB6\xBD\xB6\xBE\xB6\xBF\xB6\xC0\xB6\xC1\xB6\xC2\xB6\xC3\xB6\xC4"
    "\xB6\xC5\xB6\xC6\xB6\xC7\xB6\xC8\xB6\xC9\xB6\xCA\xB6\xCB\xB6\xCC\xB6\xCD\xB6\xCE\xB6\xCF\xB6\xD0\xB6\xD1\xB6\xD2\xB6\xD3\xB6\xD4\xB6\xD5\xB6\xD6\xB6\xD7\xB6\xD8\xB6\xD9\xB6\xDA\xB6\xDB\xB6\xDC\xB6\xDD"
    "\xB6\xDE\xB6\xDF\xB6\xE0\xB6\xE1\xB6\xE2\xB6\xE3\xB6\xE4\xB6\xE5\xB6\xE6\xB6\xE7\xB6\xE8\xB6\xE9\xB6\xEA\xB6\xEB\xB6\xEC\xB6\xED\xB6\xEE\xB6\xEF\xB6\xF0\xB6\xF1\xB6\xF2\xB6\xF3\xB6\xF4\xB6\xF5\xB6\xF6"
    "\xB6\xF7\xB6\xF8\xB6\xF9\xB6\xFA\xB6\xFB\xB6\xFC\xB6\xFD\xB6\xFE\xB7\xA1\xB7\xA2\xB7\xA3\xB7\xA4\xB7\xA5\xB7\xA6\xB7\xA7\xB7\xA8\xB7\xA9\xB7\xAA\xB7\xAB\xB7\xAC\xB7\xAD\xB7\xAE\xB7\xAF\xB7\xB0\xB7\xB1"
    "\xB7\xB2\xB7\xB3\xB7\xB4\xB7\xB5\xB7\xB6\xB7\xB7\xB7\xB8\xB7\xB9\xB7\xBA\xB7\xBB\xB7\xBC\xB7\xBD\xB7\xBE\xB7\xBF\xB7\xC0\xB7\xC1\xB7\xC2\xB7\xC3\xB7\xC4\xB7\xC5\xB7\xC6\xB7\xC7\xB7\xC8\xB7\xC9\xB7\xCA"
    "\xB7\xCB\xB7\xCC\xB7\xCD\xB7\xCE\xB7\xCF\xB7\xD0\xB7\xD1\xB7\xD2\xB7\xD3\xB7\xD4\xB7\xD5\xB7\xD6\xB7\xD7\xB7\xD8\xB7\xD9\xB7\xDA\xB7\xDB\xB7\xDC\xB7\xDD\xB7\xDE\xB7\xDF\xB7\xE0\xB7\xE1\xB7\xE2\xB7\xE3"
    "\xB7\xE4\xB7\xE5\xB7\xE6\xB7\xE7\xB7\xE8\xB7\xE9\xB7\xEA\xB7\xEB\xB7\xEC\xB7\xED\xB7\xEE\xB7\xEF\xB7\xF0\xB7\xF1\xB7\xF2\xB7\xF3\xB7\xF4\xB7\xF5\xB7\xF6\xB7\xF7\xB7\xF8\xB7\xF9\xB7\xFA\xB7\xFB\xB7\xFC"
    "\xB7\xFD\xB7\xFE\xB8\xA1\xB8\xA2\xB8\xA3\xB8\xA4\xB8\xA5\xB8\xA6\xB8\xA7\xB8\xA8\xB8\xA9\xB8\xAA\xB8\xAB\xB8\xAC\xB8\xAD\xB8\xAE\xB8\xAF\xB8\xB0\xB8\xB1\xB8\xB2\xB8\xB3\xB8\xB4\xB8\xB5\xB8\xB6\xB8\xB7"
    "\xB8\xB8\xB8\xB9\xB8\xBA\xB8\xBB\xB8\xBC\xB8\xBD\xB8\xBE\xB8\xBF\xB8\xC0\xB8\xC1\xB8\xC2\xB8\xC3\xB8\xC4\xB8\xC5\xB8\xC6\xB8\xC7\xB8\xC8\xB8\xC9\xB8\xCA\xB8\xCB\xB8\xCC\xB8\xCD\xB8\xCE\xB8\xCF\xB8\xD0"
    "\xB8\xD1\xB8\xD2\xB8\xD3\xB8\xD4\xB8\xD5\xB8\xD6\xB8\xD7\xB8\xD8\xB8\xD9\xB8\xDA\xB8\xDB\xB8\xDC\xB8\xDD\xB8\xDE\xB8\xDF\xB8\xE0\xB8\xE1\xB8\xE2\xB8\xE3\xB8\xE4\xB8\xE5\xB8\xE6\xB8\xE7\xB8\xE8\xB8\xE9"
    "\xB8\xEA\xB8\xEB\xB8\xEC\xB8\xED\xB8\xEE\xB8\xEF\xB8\xF0\xB8\xF1\xB8\xF2\xB8\xF3\xB8\xF4\xB8\xF5\xB8\xF6\xB8\xF7\xB8\xF8\xB8\xF9\xB8\xFA\xB8\xFB\xB8\xFC\xB8\xFD\xB8\xFE\xB9\xA1\xB9\xA2\xB9\xA3\xB9\xA4"
    "\xB9\xA5\xB9\xA6\xB9\xA7\xB9\xA8\xB9\xA9\xB9\xAA\xB9\xAB\xB9\xAC\xB9\xAD\xB9\xAE\xB9\xAF\xB9\xB0\xB9\xB1\xB9\xB2\xB9\xB3\xB9\xB4\xB9\xB5\xB9\xB6\xB9\xB7\xB9\xB8\xB9\xB9\xB9\xBA\xB9\xBB\xB9\xBC\xB9\xBD"
    "\xB9\xBE\xB9\xBF\xB9\xC0\xB9\xC1\xB9\xC2\xB9\xC3\xB9\xC4\xB9\xC5\xB9\xC6\xB9\xC7\xB9\xC8\xB9\xC9\xB9\xCA\xB9\xCB\xB9\xCC\xB9\xCD\xB9\xCE\xB9\xCF\xB9\xD0\xB9\xD1\xB9\xD2\xB9\xD3\xB9\xD4\xB9\xD5\xB9\xD6"
    "\xB9\xD7\xB9\xD8\xB9\xD9\xB9\xDA\xB9\xDB\xB9\xDC\xB9\xDD\xB9\xDE\xB9\xDF\xB9\xE0\xB9\xE1\xB9\xE2\xB9\xE3\xB9\xE4\xB9\xE5\xB9\xE6\xB9\xE7\xB9\xE8\xB9\xE9\xB9\xEA\xB9\xEB\xB9\xEC\xB9\xED\xB9\xEE\xB9\xEF"
    "\xB9\xF0\xB9\xF1\xB9\xF2\xB9\xF3\xB9\xF4\xB9\xF5\xB9\xF6\xB9\xF7\xB9\xF8\xB9\xF9\xB9\xFA\xB9\xFB\xB9\xFC\xB9\xFD\xB9\xFE\xBA\xA1\xBA\xA2\xBA\xA3\xBA\xA4\xBA\xA5\xBA\xA6\xBA\xA7\xBA\xA8\xBA\xA9\xBA\xAA"
    "\xBA\xAB\xBA\xAC\xBA\xAD\xBA\xAE\xBA\xAF\xBA\xB0\xBA\xB1\xBA\xB2\xBA\xB3\xBA\xB4\xBA\xB5\xBA\xB6\xBA\xB7\xBA\xB8\xBA\xB9\xBA\xBA\xBA\xBB\xBA\xBC\xBA\xBD\xBA\xBE\xBA\xBF\xBA\xC0\xBA\xC1\xBA\xC2\xBA\xC3"
    "\xBA\xC4\xBA\xC5\xBA\xC6\xBA\xC7\xBA\xC8\xBA\xC9\xBA\xCA\xBA\xCB\xBA\xCC\xBA\xCD\xBA\xCE\xBA\xCF\xBA\xD0\xBA\xD1\xBA\xD2\xBA\xD3\xBA\xD4\xBA\xD5\xBA\xD6\xBA\xD7\xBA\xD8\xBA\xD9\xBA\xDA\xBA\xDB\xBA\xDC"
    "\xBA\xDD\xBA\xDE\xBA\xDF\xBA\xE0\xBA\xE1\xBA\xE2\xBA\xE3\xBA\xE4\xBA\xE5\xBA\xE6\xBA\xE7\xBA\xE8\xBA\xE9\xBA\xEA\xBA\xEB\xBA\xEC\xBA\xED\xBA\xEE\xBA\xEF\xBA\xF0\xBA\xF1\xBA\xF2\xBA\xF3\xBA\xF4\xBA\xF5"
    "\xBA\xF6\xBA\xF7\xBA\xF8\xBA\xF9\xBA\xFA\xBA\xFB\xBA\xFC\xBA\xFD\xBA\xFE\xBB\xA1\xBB\xA2\xBB\xA3\xBB\xA4\xBB\xA5\xBB\xA6\xBB\xA7\xBB\xA8\xBB\xA9\xBB\xAA\xBB\xAB\xBB\xAC\xBB\xAD\xBB\xAE\xBB\xAF\xBB\xB0"
    "\xBB\xB1\xBB\xB2\xBB\xB3\xBB\xB4\xBB\xB5\xBB\xB6\xBB\xB7\xBB\xB8\xBB\xB9\xBB\xBA\xBB\xBB\xBB\xBC\xBB\xBD\xBB\xBE\xBB\xBF\xBB\xC0\xBB\xC1\xBB\xC2\xBB\xC3\xBB\xC4\xBB\xC5\xBB\xC6\xBB\xC7\xBB\xC8\xBB\xC9"
    "\xBB\xCA\xBB\xCB\xBB\xCC\xBB\xCD\xBB\xCE\xBB\xCF\xBB\xD0\xBB\xD1\xBB\xD2\xBB\xD3\xBB\xD4\xBB\xD5\xBB\xD6\xBB\xD7\xBB\xD8\xBB\xD9\xBB\xDA\xBB\xDB\xBB\xDC\xBB\xDD\xBB\xDE\xBB\xDF\xBB\xE0\xBB\xE1\xBB\xE2"
    "\xBB\xE3\xBB\xE4\xBB\xE5\xBB\xE6\xBB\xE7\xBB\xE8\xBB\xE9\xBB\xEA\xBB\xEB\xBB\xEC\xBB\xED\xBB\xEE\xBB\xEF\xBB\xF0\xBB\xF1\xBB\xF2\xBB\xF3\xBB\xF4\xBB\xF5\xBB\xF6\xBB\xF7\xBB\xF8\xBB\xF9\xBB\xFA\xBB\xFB"
    "\xBB\xFC\xBB\xFD\xBB\xFE\xBC\xA1\xBC\xA2\xBC\xA3\xBC\xA4\xBC\xA5\xBC\xA6\xBC\xA7\xBC\xA8\xBC\xA9\xBC\xAA\xBC\xAB\xBC\xAC\xBC\xAD\xBC\xAE\xBC\xAF\xBC\xB0\xBC\xB1\xBC\xB2\xBC\xB3\xBC\xB4\xBC\xB5\xBC\xB6"
    "\xBC\xB7\xBC\xB8\xBC\xB9\xBC\xBA\xBC\xBB\xBC\xBC\xBC\xBD\xBC\xBE\xBC\xBF\xBC\xC0\xBC\xC1\xBC\xC2\xBC\xC3\xBC\xC4\xBC\xC5\xBC\xC6\xBC\xC7\xBC\xC8\xBC\xC9\xBC\xCA\xBC\xCB\xBC\xCC\xBC\xCD\xBC\xCE\xBC\xCF"
    "\xBC\xD0\xBC\xD1\xBC\xD2\xBC\xD3\xBC\xD4\xBC\xD5\xBC\xD6\xBC\xD7\xBC\xD8\xBC\xD9\xBC\xDA\xBC\xDB\xBC\xDC\xBC\xDD\xBC\xDE\xBC\xDF\xBC\xE0\xBC\xE1\xBC\xE2\xBC\xE3\xBC\xE4\xBC\xE5\xBC\xE6\xBC\xE7\xBC\xE8"
    "\xBC\xE9\xBC\xEA\xBC\xEB\xBC\xEC\xBC\xED\xBC\xEE\xBC\xEF\xBC\xF0\xBC\xF1\xBC\xF2\xBC\xF3\xBC\xF4\xBC\xF5\xBC\xF6\xBC\xF7\xBC\xF8\xBC\xF9\xBC\xFA\xBC\xFB\xBC\xFC\xBC\xFD\xBC\xFE\xBD\xA1\xBD\xA2\xBD\xA3"
    "\xBD\xA4\xBD\xA5\xBD\xA6\xBD\xA7\xBD\xA8\xBD\xA9\xBD\xAA\xBD\xAB\xBD\xAC\xBD\xAD\xBD\xAE\xBD\xAF\xBD\xB0\xBD\xB1\xBD\xB2\xBD\xB3\xBD\xB4\xBD\xB5\xBD\xB6\xBD\xB7\xBD\xB8\xBD\xB9\xBD\xBA\xBD\xBB\xBD\xBC"
    "\xBD\xBD\xBD\xBE\xBD\xBF\xBD\xC0\xBD\xC1\xBD\xC2\xBD\xC3\xBD\xC4\xBD\xC5\xBD\xC6\xBD\xC7\xBD\xC8\xBD\xC9\xBD\xCA\xBD\xCB\xBD\xCC\xBD\xCD\xBD\xCE\xBD\xCF\xBD\xD0\xBD\xD1\xBD\xD2\xBD\xD3\xBD\xD4\xBD\xD5"
    "\xBD\xD6\xBD\xD7\xBD\xD8\xBD\xD9\xBD\xDA\xBD\xDB\xBD\xDC\xBD\xDD\xBD\xDE\xBD\xDF\xBD\xE0\xBD\xE1\xBD\xE2\xBD\xE3\xBD\xE4\xBD\xE5\xBD\xE6\xBD\xE7\xBD\xE8\xBD\xE9\xBD\xEA\xBD\xEB\xBD\xEC\x9B\xC0\xBD\xEE"
    "\xBD\xEF\xBD\xF0\xBD\xF1\xBD\xF2\xBD\xF3\xBD\xF4\xBD\xF5\xBD\xF6\xBD\xF7\xBD\xF8\xBD\xF9\xBD\xFA\xBD\xFB\xBD\xFC\xBD\xFD\xBD\xFE\xBE\xA1\xBE\xA2\xBE\xA3\xBE\xA4\xBE\xA5\xBE\xA6\xBE\xA7\xBE\xA8\xBE\xA9"
    "\xBE\xAA\xBE\xAB\xBE\xAC\xBE\xAD\xBE\xAE\xBE\xAF\xBE\xB0\xBE\xB1\xBE\xB2\xBE\xB3\xBE\xB4\xBE\xB5\xBE\xB6\xBE\xB7\xBE\xB8\xBE\xB9\xBE\xBA\xBE\xBB\xBE\xBC\xBE\xBD\xBE\xBE\xBE\xBF\xBE\xC0\xBE\xC1\xBE\xC2"
    "\xBE\xC3\xBE\xC4\xBE\xC5\xBE\xC6\xBE\xC7\xBE\xC8\xBE\xC9\xBE\xCA\xBE\xCB\xBE\xCC\xBE\xCD\xBE\xCE\xBE\xCF\xBE\xD0\xBE\xD1\xBE\xD2\xBE\xD3\xBE\xD4\xBE\xD5\xBE\xD6\xBE\xD7\xBE\xD8\xBE\xD9\xBE\xDA\xBE\xDB"
    "\xBE\xDC\xBE\xDD\xBE\xDE\xBE\xDF\xBE\xE0\xBE\xE1\xBE\xE2\xBE\xE3\xBE\xE4\xBE\xE5\xBE\xE6\xBE\xE7\xBE\xE8\xBE\xE9\xBE\xEA\xBE\xEB\xBE\xEC\xBE\xED\xBE\xEE\xBE\xEF\xBE\xF0\xBE\xF1\xBE\xF2\xBE\xF3\xBE\xF4"
    "\xBE\xF5\xBE\xF6\xBE\xF7\xBE\xF8\xBE\xF9\xBE\xFA\xBE\xFB\xBE\xFC\xBE\xFD\xBE\xFE\xBF\xA1\xBF\xA2\xBF\xA3\xBF\xA4\xBF\xA5\xBF\xA6\xBF\xA7\xBF\xA8\xBF\xA9\xBF\xAA\xBF\xAB\xBF\xAC\xBF\xAD\xBF\xAE\xBF\xAF"
    "\xBF\xB0\xBF\xB1\xBF\xB2\xBF\xB3\xBF\xB4\xBF\xB5\xBF\xB6\xBF\xB7\xBF\xB8\xBF\xB9\xBF\xBA\xBF\xBB\xBF\xBC\xBF\xBD\xBF\xBE\xBF\xBF\xBF\xC0\xBF\xC1\xBF\xC2\xBF\xC3\xBF\xC4\xBF\xC5\xBF\xC6\xBF\xC7\xBF\xC8"
    "\xBF\xC9\xBF\xCA\xBF\xCB\xBF\xCC\xBF\xCD\xBF\xCE\xBF\xCF\xBF\xD0\xBF\xD1\xBF\xD2\xBF\xD3\xBF\xD4\xBF\xD5\xBF\xD6\xBF\xD7\xBF\xD8\xBF\xD9\xBF\xDA\xBF\xDB\xBF\xDC\xBF\xDD\xBF\xDE\xBF\xDF\xBF\xE0\xBF\xE1"
    "\xBF\xE2\xBF\xE3\xBF\xE4\xBF\xE5\xBF\xE6\xBF\xE7\xBF\xE8\xBF\xE9\xBF\xEA\xBF\xEB\xBF\xEC\xBF\xED\xBF\xEE\xBF\xEF\xBF\xF0\xBF\xF1\xBF\xF2\xBF\xF3\xBF\xF4\xBF\xF5\xBF\xF6\xBF\xF7\xBF\xF8\xBF\xF9\xBF\xFA"
    "\xBF\xFB\xBF\xFC\xBF\xFD\xBF\xFE\xC0\xA1\xC0\xA2\xC0\xA3\xC0\xA4\xC0\xA5\xC0\xA6\xC0\xA7\xC0\xA8\xC0\xA9\xC0\xAA\xC0\xAB\xC0\xAC\xC0\xAD\xC0\xAE\xC0\xAF\xC0\xB0\xC0\xB1\xC0\xB2\xC0\xB3\xC0\xB4\xC0\xB5"
    "\xC0\xB6\xC0\xB7\xC0\xB8\xC0\xB9\xC0\xBA\xC0\xBB\xC0\xBC\xC0\xBD\xC0\xBE\xC0\xBF\xC0\xC0\xC0\xC1\xC0\xC2\xC0\xC3\xC0\xC4\xC0\xC5\xC0\xC6\xC0\xC7\xC0\xC8\xC0\xC9\xC0\xCA\xC0\xCB\xC0\xCC\xC0\xCD\xC0\xCE"
    "\xC0\xCF\xC0\xD0\xC0\xD1\xC0\xD2\xC0\xD3\xC0\xD4\xC0\xD5\xC0\xD6\xC0\xD7\xC0\xD8\xC0\xD9\xC0\xDA\xC0\xDB\xC0\xDC\xC0\xDD\xC0\xDE\xC0\xDF\xC0\xE0\xC0\xE1\xC0\xE2\xC0\xE3\xC0\xE4\xC0\xE5\xC0\xE6\xC0\xE7"
    "\xC0\xE8\xC0\xE9\xC0\xEA\xC0\xEB\xC0\xEC\xC0\xED\xC0\xEE\xC0\xEF\xC0\xF0\xC0\xF1\xC0\xF2\xC0\xF3\xC0\xF4\xC0\xF5\xC0\xF6\xC0\xF7\xC0\xF8\xC0\xF9\xC0\xFA\xC0\xFB\xC0\xFC\xC0\xFD\xC0\xFE\xC1\xA1\xC1\xA2"
    "\xC1\xA3\xC1\xA4\xC1\xA5\xC1\xA6\xC1\xA7\xC1\xA8\xC1\xA9\xC1\xAA\xC1\xAB\xC1\xAC\xC1\xAD\xC1\xAE\xC1\xAF\xC1\xB0\xC1\xB1\xC1\xB2\xC1\xB3\xC1\xB4\xC1\xB5\xC1\xB6\xC1\xB7\xC1\xB8\xC1\xB9\xC1\xBA\xC1\xBB"
    "\xC1\xBC\xC1\xBD\xC1\xBE\xC1\xBF\xC1\xC0\xC1\xC1\xC1\xC2\xC1\xC3\xC1\xC4\xC1\xC5\xC1\xC6\xC1\xC7\xC1\xC8\xC1\xC9\xC1\xCA\xC1\xCB\xC1\xCC\xC1\xCD\xC1\xCE\xC1\xCF\xC1\xD0\xC1\xD1\xC1\xD2\xC1\xD3\xC1\xD4"
    "\xC1\xD5\xC1\xD6\xC1\xD7\xC1\xD8\xC1\xD9\xC1\xDA\xC1\xDB\xC1\xDC\xC1\xDD\xC1\xDE\xC1\xDF\xC1\xE0\xC1\xE1\xC1\xE2\xC1\xE3\xC1\xE4\xC1\xE5\xC1\xE6\xC1\xE7\xC1\xE8\xC1\xE9\xC1\xEA\xC1\xEB\xC1\xEC\xC1\xED"
    "\xC1\xEE\xC1\xEF\xC1\xF0\xC1\xF1\xC1\xF2\xC1\xF3\xC1\xF4\xC1\xF5\xC1\xF6\xC1\xF7\xC1\xF8\xC1\xF9\xC1\xFA\xC1\xFB\xC1\xFC\xC1\xFD\xC1\xFE\xC2\xA1\xC2\xA2\xC2\xA3\xC2\xA4\xC2\xA5\xC2\xA6\xC2\xA7\xC2\xA8"
    "\xC2\xA9\xC2\xAA\xC2\xAB\xC2\xAC\xC2\xAD\xC2\xAE\xC2\xAF\xC2\xB0\xC2\xB1\xC2\xB2\xC2\xB3\xC2\xB4\xC2\xB5\xC2\xB6\xC2\xB7\xC2\xB8\xC2\xB9\xC2\xBA\xC2\xBB\xC2\xBC\xC2\xBD\xC2\xBE\xC2\xBF\xC2\xC0\xC2\xC1"
    "\xC2\xC2\xC2\xC3\xC2\xC4\xC2\xC5\xC2\xC6\xC2\xC7\xC2\xC8\xC2\xC9\xC2\xCA\xC2\xCB\xC2\xCC\xC2\xCD\xC2\xCE\xC2\xCF\xC2\xD0\xC2\xD1\xC2\xD2\xC2\xD3\xC2\xD4\xC2\xD5\xC2\xD6\xC2\xD7\xC2\xD8\xC2\xD9\xC2\xDA"
    "\xC2\xDB\xC2\xDC\xC2\xDD\xC2\xDE\xC2\xDF\xC2\xE0\xC2\xE1\xC2\xE2\xC2\xE3\xC2\xE4\xC2\xE5\xC2\xE6\xC2\xE7\xC2\xE8\xC2\xE9\xC2\xEA\xC2\xEB\xC2\xEC\xC2\xED\xC2\xEE\xC2\xEF\xC2\xF0\xC2\xF1\xC2\xF2\xC2\xF3"
    "\xC2\xF4\xC2\xF5\xC2\xF6\xC2\xF7\xC2\xF8\xC2\xF9\xC2\xFA\xC2\xFB\xC2\xFC\xC2\xFD\xC2\xFE\xC3\xA1\xC3\xA2\xC3\xA3\xC3\xA4\xC3\xA5\xC3\xA6\xC3\xA7\xC3\xA8\xC3\xA9\xC3\xAA\xC3\xAB\xC3\xAC\xC3\xAD\xC3\xAE"
    "\xC3\xAF\xC3\xB0\xC3\xB1\xC3\xB2\xC3\xB3\xC3\xB4\xC3\xB5\xC3\xB6\xC3\xB7\xC3\xB8\xC3\xB9\xC3\xBA\xC3\xBB\xC3\xBC\xC3\xBD\xC3\xBE\xC3\xBF\xC3\xC0\xC3\xC1\xC3\xC2\xC3\xC3\xC3\xC4\xC3\xC5\xC3\xC6\xC3\xC7"
    "\xC3\xC8\xC3\xC9\xC3\xCA\xC3\xCB\xC3\xCC\xC3\xCD\xC3\xCE\xC3\xCF\xC3\xD0\xC3\xD1\xC3\xD2\xC3\xD3\xC3\xD4\xC3\xD5\xC3\xD6\xC3\xD7\xC3\xD8\xC3\xD9\xC3\xDA\xC3\xDB\xC3\xDC\xC3\xDD\xC3\xDE\xC3\xDF\xC3\xE0"
    "\xC3\xE1\xC3\xE2\xC3\xE3\xC3\xE4\xC3\xE5\xC3\xE6\xC3\xE7\xC3\xE8\xC3\xE9\xC3\xEA\xC3\xEB\xC3\xEC\xC3\xED\xC3\xEE\xC3\xEF\xC3\xF0\xC3\xF1\xC3\xF2\xC3\xF3\xC3\xF4\xC3\xF5\xC3\xF6\xC3\xF7\xC3\xF8\xC3\xF9"
    "\xC3\xFA\xC3\xFB\xC3\xFC\xC3\xFD\xC3\xFE\xC4\xA1\xC4\xA2\xC4\xA3\xC4\xA4\xC4\xA5\xC4\xA6\xC4\xA7\xC4\xA8\xC4\xA9\xC4\xAA\xC4\xAB\xC4\xAC\xC4\xAD\xC4\xAE\xC4\xAF\xC4\xB0\xC4\xB1\xC4\xB2\xC4\xB3\xC4\xB4"
    "\xC4\xB5\xC4\xB6\xC4\xB7\xC4\xB8\xC4\xB9\xC4\xBA\xC4\xBB\xC4\xBC\xC4\xBD\xC4\xBE\xC4\xBF\xC4\xC0\xC4\xC1\xC4\xC2\xC4\xC3\xC4\xC4\xC4\xC5\xC4\xC6\xC4\xC7\xC4\xC8\xC4\xC9\xC4\xCA\xC4\xCB\xC4\xCC\xC4\xCD"
    "\xC4\xCE\xC4\xCF\xC4\xD0\xC4\xD1\xC4\xD2\xC4\xD3\xC4\xD4\xC4\xD5\xC4\xD6\xC4\xD7\xC4\xD8\xC4\xD9\xC4\xDA\xC4\xDB\xC4\xDC\xC4\xDD\xC4\xDE\xC4\xDF\xC4\xE0\xC4\xE1\xC4\xE2\xC4\xE3\xC4\xE4\xC4\xE5\xC4\xE6"
    "\xC4\xE7\xC4\xE8\xC4\xE9\xC4\xEA\xC4\xEB\xC4\xEC\xC4\xED\xC4\xEE\xC4\xEF\xC4\xF0\xC4\xF1\xC4\xF2\xC4\xF3\xC4\xF4\xC4\xF5\xC4\xF6\xC4\xF7\xC4\xF8\xC4\xF9\xC4\xFA\xC4\xFB\xC4\xFC\xC4\xFD\xC4\xFE\xC5\xA1"
    "\xC5\xA2\xC5\xA3\xC5\xA4\xC5\xA5\xC5\xA6\xC5\xA7\xC5\xA8\xC5\xA9\xC5\xAA\xC5\xAB\xC5\xAC\xC5\xAD\xC5\xAE\xC5\xAF\xC5\xB0\xC5\xB1\xC5\xB2\xC5\xB3\xC5\xB4\xC5\xB5\xC5\xB6\xC5\xB7\xC5\xB8\xC5\xB9\xC5\xBA"
    "\xC5\xBB\xC5\xBC\xC5\xBD\xC5\xBE\xC5\xBF\xC5\xC0\xC5\xC1\xC5\xC2\xC5\xC3\xC5\xC4\xC5\xC5\xC5\xC6\xC5\xC7\xC5\xC8\xC5\xC9\xC5\xCA\xC5\xCB\xC5\xCC\xC5\xCD\xC5\xCE\xC5\xCF\xC5\xD0\xC5\xD1\xC5\xD2\xC5\xD3"
    "\xC5\xD4\xC5\xD5\xC5\xD6\xC5\xD7\xC5\xD8\xC5\xD9\xC5\xDA\xC5\xDB\xC5\xDC\xC5\xDD\xC5\xDE\xC5\xDF\xC5\xE0\xC5\xE1\xC5\xE2\xC5\xE3\xC5\xE4\xC5\xE5\xC5\xE6\xC5\xE7\xC5\xE8\xC5\xE9\xC5\xEA\xC5\xEB\xC5\xEC"
    "\xC5\xED\xC5\xEE\xC5\xEF\xC5\xF0\xC5\xF1\xC5\xF2\xC5\xF3\xC5\xF4\xC5\xF5\xC5\xF6\xC5\xF7\xC5\xF8\xC5\xF9\xC5\xFA\xC5\xFB\xC5\xFC\xC5\xFD\xC5\xFE\xC6\xA1\xC6\xA2\xC6\xA3\xC6\xA4\xC6\xA5\xC6\xA6\xC6\xA7"
    "\xC6\xA8\xC6\xA9\xC6\xAA\xC6\xAB\xC6\xAC\xC6\xAD\xC6\xAE\xC6\xAF\xC6\xB0\xC6\xB1\xC6\xB2\xC6\xB3\xC6\xB4\xC6\xB5\xC6\xB6\xC6\xB7\xC6\xB8\xC6\xB9\xC6\xBA\xC6\xBB\xC6\xBC\xC6\xBD\xC6\xBE\xC6\xBF\xC6\xC0"
    "\xC6\xC1\xC6\xC2\xC6\xC3\xC6\xC4\xC6\xC5\xC6\xC6\xC6\xC7\xC6\xC8\xC6\xC9\xC6\xCA\xC6\xCB\xC6\xCC\xC6\xCD\xC6\xCE\xC6\xCF\xC6\xD0\xC6\xD1\xC6\xD2\xC6\xD3\xC6\xD4\xC6\xD5\xC6\xD6\xC6\xD7\xC6\xD8\xC6\xD9"
    "\xC6\xDA\xC6\xDB\xC6\xDC\xC6\xDD\xC6\xDE\xC6\xDF\xC6\xE0\xC6\xE1\xC6\xE2\xC6\xE3\xC6\xE4\xC6\xE5\xC6\xE6\xC6\xE7\xC6\xE8\xC6\xE9\xC6\xEA\xC6\xEB\xC6\xEC\xC6\xED\xC6\xEE\xC6\xEF\xC6\xF0\xC6\xF1\xC6\xF2"
    "\xC6\xF3\xC6\xF4\xC6\xF5\xC6\xF6\xC6\xF7\xC6\xF8\xC6\xF9\xC6\xFA\xC6\xFB\xC6\xFC\xC6\xFD\xC6\xFE\xC7\xA1\xC7\xA2\xC7\xA3\xC7\xA4\xC7\xA5\xC7\xA6\xC7\xA7\xC7\xA8\xC7\xA9\xC7\xAA\xC7\xAB\xC7\xAC\xC7\xAD"
    "\xC7\xAE\xC7\xAF\xC7\xB0\xC7\xB1\xC7\xB2\xC7\xB3\xC7\xB4\xC7\xB5\xC7\xB6\xC7\xB7\xC7\xB8\xC7\xB9\xC7\xBA\xC7\xBB\xC7\xBC\xC7\xBD\xC7\xBE\xC7\xBF\xC7\xC0\xC7\xC1\xC7\xC2\xC7\xC3\xC7\xC4\xC7\xC5\xC7\xC6"
    "\xC7\xC7\xC7\xC8\xC7\xC9\xC7\xCA\xC7\xCB\xC7\xCC\xC7\xCD\xC7\xCE\xC7\xCF\xC7\xD0\xC7\xD1\xC7\xD2\xC7\xD3\xC7\xD4\xC7\xD5\xC7\xD6\xC7\xD7\xC7\xD8\xC7\xD9\xC7\xDA\xC7\xDB\xC7\xDC\xC7\xDD\xC7\xDE\xC7\xDF"
    "\xC7\xE0\xC7\xE1\xC7\xE2\xC7\xE3\xC7\xE4\xC7\xE5\xC7\xE6\xC7\xE7\xC7\xE8\xC7\xE9\xC7\xEA\xC7\xEB\xC7\xEC\xC7\xED\xC7\xEE\xC7\xEF\xC7\xF0\xC7\xF1\xC7\xF2\xC7\xF3\xC7\xF4\xC7\xF5\xC7\xF6\xC7\xF7\xC7\xF8"
    "\xC7\xF9\xC7\xFA\xC7\xFB\xC7\xFC\xC7\xFD\xC7\xFE\xC8\xA1\xC8\xA2\xC8\xA3\xC8\xA4\xC8\xA5\xC8\xA6\xC8\xA7\xC8\xA8\xC8\xA9\xC8\xAA\xC8\xAB\xC8\xAC\xC8\xAD\xC8\xAE\xC8\xAF\xC8\xB0\xC8\xB1\xC8\xB2\xC8\xB3"
    "\xC8\xB4\xC8\xB5\xC8\xB6\xC8\xB7\xC8\xB8\xC8\xB9\xC8\xBA\xC8\xBB\xC8\xBC\xC8\xBD\xC8\xBE\xC8\xBF\xC8\xC0\xC8\xC1\xC8\xC2\xC8\xC3\xC8\xC4\xC8\xC5\xC8\xC6\xC8\xC7\xC8\xC8\xC8\xC9\xC8\xCA\xC8\xCB\xC8\xCC"
    "\xC8\xCD\xC8\xCE\xC8\xCF\xC8\xD0\xC8\xD1\xC8\xD2\xC8\xD3\xC8\xD4\xC8\xD5\xC8\xD6\xC8\xD7\xC8\xD8\xC8\xD9\xC8\xDA\xC8\xDB\xC8\xDC\xC8\xDD\xC8\xDE\xC8\xDF\xC8\xE0\xC8\xE1\xC8\xE2\xC8\xE3\xC8\xE4\xC8\xE5"
    "\xC8\xE6\xC8\xE7\xC8\xE8\xC8\xE9\xC8\xEA\xC8\xEB\xC8\xEC\xC8\xED\xC8\xEE\xC8\xEF\xC8\xF0\xC8\xF1\xC8\xF2\xC8\xF3\xC8\xF4\xC8\xF5\xC8\xF6\xC8\xF7\xC8\xF8\xC8\xF9\xC8\xFA\xC8\xFB\xC8\xFC\xC8\xFD\xC8\xFE";
