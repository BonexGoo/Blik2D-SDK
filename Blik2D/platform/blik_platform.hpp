#pragma once
#include <blik.hpp>
#include <element/blik_color.hpp>
#include <element/blik_point.hpp>
#include <element/blik_vector.hpp>

namespace BLIK
{
    enum UIRole {UIR_Menu = 0x1, UIR_Tool = 0x2, UIR_Both = 0x3};
    static UIRole operator|(UIRole a, UIRole b) {return (UIRole) (int(a) | int(b));}
    enum UIAllow {UIA_Left = 0x1, UIA_Top = 0x2, UIA_Right = 0x4, UIA_Bottom = 0x8, UIA_Wherever = 0xF};
    static UIAllow operator|(UIAllow a, UIAllow b) {return (UIAllow) (int(a) | int(b));}
    enum UIAlign {
        UIA_LeftTop,    UIA_CenterTop,    UIA_RightTop,
        UIA_LeftMiddle, UIA_CenterMiddle, UIA_RightMiddle,
        UIA_LeftBottom, UIA_CenterBottom, UIA_RightBottom};
    enum UIFontAlign {
        UIFA_LeftTop,    UIFA_CenterTop,    UIFA_RightTop,    UIFA_JustifyTop,
        UIFA_LeftMiddle, UIFA_CenterMiddle, UIFA_RightMiddle, UIFA_JustifyMiddle,
        UIFA_LeftAscent, UIFA_CenterAscent, UIFA_RightAscent, UIFA_JustifyAscent,
        UIFA_LeftBottom, UIFA_CenterBottom, UIFA_RightBottom, UIFA_JustifyBottom};
    enum UIFontElide {UIFE_None, UIFE_Left, UIFE_Center, UIFE_Right};
    enum UIEditType {UIET_String, UIET_Int, UIET_Float};
    enum UITestOrder {UITO_ScissorOn, UITO_ScissorOff};
    enum UIStack {UIS_PushPop, UIS_Push, UIS_Current, UIS_Pop};
    class ViewClass;
    class ViewManager;
    typedef bool (*PassCB)(void*, payload);
	typedef void (*ThreadCB)(void*);
	typedef sint32 (*SerialDecodeCB)(bytes data, sint32 length, uint08s& outdata, sint32* outtype);
	typedef void (*SerialEncodeCB)(bytes data, sint32 length, uint08s& outdata, sint32 type);

    //! \brief 크로스플랫폼지원
    class Platform
    {
    public:
        /*!
        \brief 플랫폼초기화(GL뷰)
        */
        static void InitForGL();

        /*!
        \brief 플랫폼초기화(MDI뷰)
        */
        static void InitForMDI();

        /*!
        \brief 윈도우이름 설정
        \param name : 윈도우이름
        */
        static void SetWindowName(chars name);

		/*!
		\brief 윈도우위치 설정
		\param x : 가로위치(px)
		\param y : 세로위치(px)
		*/
		static void SetWindowPos(sint32 x, sint32 y);

        /*!
        \brief 윈도우사이즈 설정
        \param width : 가로길이(px)
        \param height : 세로길이(px)
        */
        static void SetWindowSize(sint32 width, sint32 height);

		/*!
		\brief 윈도우영역 얻기
		\param rect : 윈도우영역(px)
		*/
		static void GetWindowRect(rect128& rect);

        /*!
        \brief 스크린영역 얻기
        \param rect : 스크린영역(px)
        \return 물리적모니터 연결여부(HDMI접속상태등으로 판단)
        */
        static bool GetScreenRect(rect128& rect);

        /*!
        \brief 윈도우뷰 설정
        \param viewclass : 로드할 뷰클래스(BLIK_DECLARE_VIEW로 선언, nullptr일 경우 _defaultview_)
		\return 뷰핸들
        */
        static h_view SetWindowView(chars viewclass = nullptr);

        /*!
        \brief 상태창 설정
        \param text : 스트링
        \param stack : 스택처리
        */
        static void SetStatusText(chars text, UIStack stack);

        /*!
        \brief 아이콘 생성
        \param filepath : 파일경로(png)
        \return 아이콘핸들
        */
        static h_icon CreateIcon(chars filepath);

        /*!
        \brief 액션 생성
        \param name : 액션이름
        \param tip : 툴팁
        \param view : 이벤트를 전달할 뷰핸들
        \param icon : 지정할 아이콘핸들
        \return 액션핸들
        */
        static h_action CreateAction(chars name, chars tip, h_view view, h_icon icon = h_icon::null());

        /*!
        \brief 사이즈정책 생성
        \param minwidth : 최소 가로길이(px)
        \param minheight : 최소 가로길이(px)
        \param maxwidth : 최대 가로길이(px)
        \param maxheight : 최대 가로길이(px)
        \return 사이즈정책핸들
        */
        static h_policy CreatePolicy(sint32 minwidth, sint32 minheight, sint32 maxwidth = 0xFFFF, sint32 maxheight = 0xFFFF);

        /*!
        \brief 뷰 생성
        \param name : 뷰이름
        \param width : 가로길이(px)
        \param height : 세로길이(px)
        \param policy : 사이즈정책핸들
        \param viewclass : 로드할 뷰클래스(BLIK_DECLARE_VIEW로 선언, nullptr일 경우 _defaultview_)
        \return 뷰핸들
        */
        static h_view CreateView(chars name, sint32 width, sint32 height, h_policy policy, chars viewclass = nullptr);

        /*!
        \brief 해당 뷰에 변경할 뷰클래스를 지정
        \param view : 뷰핸들
        \param viewclass : 변경할 뷰클래스(BLIK_DECLARE_VIEW로 선언, nullptr일 경우 _defaultview_)
        \return 뷰클래스객체
        */
        static ViewClass* SetNextViewClass(h_view view, chars viewclass = nullptr);

        /*!
        \brief 해당 뷰에 변경할 뷰매니저를 지정
        \param view : 뷰핸들
        \param viewmanager : 변경할 뷰매니저
        \return 변경 성공 여부
        */
        static bool SetNextViewManager(h_view view, ViewManager* viewmanager);

        /*!
        \brief 도킹바 생성 및 추가
        \param view : 뷰핸들
        \param allow : 도킹할 방향
        \param allowbase : 도킹이 가능한 방향
        \return 도킹바핸들
        */
        static h_dock CreateDock(h_view view, UIAllow allow, UIAllow allowbase = UIA_Wherever);

        /*!
        \brief 윈도우에 액션 추가
        \param group : 그룹이름
        \param action : 액션핸들
        \param role : 추가할 곳
        */
        static void AddAction(chars group, h_action action, UIRole role = UIR_Both);

        /*!
        \brief 윈도우에 구분선 추가
        \param group : 그룹이름
        \param role : 추가할 곳
        */
        static void AddSeparator(chars group, UIRole role = UIR_Both);

        /*!
        \brief 윈도우에 도킹바용 토글 추가
        \param group : 그룹이름
        \param dock : 도킹바핸들
        \param role : 추가할 곳
        */
        static void AddToggle(chars group, h_dock dock, UIRole role = UIR_Both);

        /*!
        \brief 차일드식 윈도우 생성 및 추가
        \param view : 뷰핸들
        \param icon : 지정할 아이콘핸들
        \return 윈도우핸들
        */
        static h_window OpenChildWindow(h_view view, h_icon icon = h_icon::null());

        /*!
        \brief 팝업식 윈도우 생성 및 추가
        \param view : 뷰핸들
        \param icon : 지정할 아이콘핸들
        \return 윈도우핸들
        */
        static h_window OpenPopupWindow(h_view view, h_icon icon = h_icon::null());

        /*!
        \brief 윈도우 제거
        \param window : 윈도우핸들
        */
        static void CloseWindow(h_window window);

        /*!
        \brief 뷰에 알림사항 전달
        \param view : 뷰핸들
        \param topic : 주제
        \param in : 전달할 공유객체
        \param needout : 전달받을 공유객체 필요여부
        \return 전달받을 공유객체
        */
        static id_cloned_share SendNotify(h_view view, chars topic, id_share in, bool needout = false);

        /*!
        \brief 다수의 뷰에 알림사항 방송
        \param topic : 주제
        \param in : 전달할 공유객체
        \param viewclass : 전달받을 뷰클래스(BLIK_DECLARE_VIEW로 선언, nullptr일 경우 전체 뷰클래스)
        */
        static void BroadcastNotify(chars topic, id_share in, chars viewclass = nullptr);

        /*!
        \brief 특정 콜백함수를 모든 뷰에 통과시킴
        \param cb : 콜백함수
        \param data : 콜백함수에 전달할 데이터
        */
        static void PassAllViews(PassCB cb, payload data);

        //! \brief 팝업개체지원
        class Popup
        {
        public:
            /*!
            \brief 텍스트 다이얄로그
            \param text : 입력(초기값 필요) 및 결과
            \param title : 창제목
            \param topic : 입력주제
            \param ispassword : 패스워드여부
            \return 유효성(확인버튼이 아닌 종료는 무효)
            */
            static bool TextDialog(String& text, chars title, chars topic, bool ispassword = false);

            /*!
            \brief 컬러 다이얄로그
            \param r : 적색-입력(초기값 필요) 및 결과
            \param g : 녹색-입력(초기값 필요) 및 결과
            \param b : 청색-입력(초기값 필요) 및 결과
            \param a : 알파-입력(초기값 필요) 및 결과
            \param title : 창제목
            \return 유효성(확인버튼이 아닌 종료는 무효)
            */
            static bool ColorDialog(uint08& r, uint08& g, uint08& b, uint08& a, chars title);

            /*!
            \brief 파일(디렉토리) 다이얄로그
            \param path : 입력(초기값 필요) 및 결과
            \param shortpath : 아이템명
            \param title : 창제목
            \param isdir : 디렉토리여부
            \return 유효성(확인버튼이 아닌 종료는 무효)
            */
            static bool FileDialog(String& path, String* shortpath, chars title, bool isdir = false);

			/*!
            \brief 웹브라우저 다이얄로그
            \param url : 접속할 URL
            */
			static void WebBrowserDialog(String url);

            /*!
            \brief 텍스트편집 트래커 열기
            \param text : 입력(초기값 필요) 및 결과
            \param type : 입력조건
            \param l : 좌측위치(px)
            \param t : 상단위치(px)
            \param r : 우측길이(px)
            \param b : 하단길이(px)
            \return 유효성(Esc로 종료되었다면 무효)
            */
            static bool OpenEditTracker(String& text, UIEditType type, sint32 l, sint32 t, sint32 r, sint32 b);

            /*!
            \brief 텍스트선택 트래커 열기
            \param textes : 표시될 스트링들
            \param l : 좌측위치(px)
            \param t : 상단위치(px)
            \param r : 우측길이(px)
            \param b : 하단길이(px)
            \return 선택된 인덱스(Esc로 종료되었다면 -1)
            */
            static sint32 OpenListTracker(Strings textes, sint32 l, sint32 t, sint32 r, sint32 b);

            /*!
            \brief 모든 트래커 닫기
            */
            static void CloseAllTracker();

            /*!
            \brief 열린 트래커 존재여부
            \return 존재여부
            */
            static bool HasOpenedTracker();

            /*!
            \brief 툴팁 보여주기
            \param text : 표시될 스트링
            */
            static void ShowToolTip(String text);

            /*!
            \brief 툴팁 감추기
            */
            static void HideToolTip();
        };

        //! \brief 유틸리티지원
        class Utility
        {
        public:
            /*!
            \brief 난수 구하기
            \return 난수
            */
            static uint32 Random();

            /*!
            \brief 슬립처리
            \param ms : 밀리초
            \param caninput : 사용자입력 가능여부
            */
            static void Sleep(sint32 ms, bool caninput);

            /*!
            \brief 프로그램종료
            */
            static void ExitProgram();

            /*!
            \brief OS의 가용메모리현황
            \param totalbytes : 전체 메모리양(바이트단위)
            \return 사용 가능한 메모리양(바이트단위)
            */
            static sint64 CurrentAvailableMemory(sint64* totalbytes = nullptr);

			/*!
			\brief 특정 콜백함수를 스레드방식으로 실행
			\param cb : 콜백함수
			\param data : 콜백함수에 전달할 데이터
			*/
			static void Threading(ThreadCB cb, payload data);

			/*!
            \brief 소속된 스레드ID 구하기
            \return 스레드ID
            */
            static uint64 CurrentThreadID();

            /*!
            \brief 현재시간 구하기
            \return 현재시간(ms)
            */
            static uint64 CurrentTimeMS();

            /*!
            \brief 클럭의 기준시각변경
            \param timestring : 변경할 시간("yyyy-MM-dd HH:mm:ss")
            */
            static void SetClockBaseTime(chars timestring);

            /*!
            \brief 현재시간으로 클럭생성
            \return 생성된 클럭
            */
            static id_clock CreateCurrentClock();

            /*!
            \brief 해당 클럭을 복사
            \param clock : 복사할 클럭
            \return 생성된 클럭
            */
            static id_clock CreateClonedClock(id_clock_read clock);

            /*!
            \brief 클럭해제
            \param clock : 해제할 클럭
            */
            static void ReleaseClock(id_clock clock);

            /*!
            \brief 클럭간 거리구하기
            \param from : 기준 클럭
            \param to : 목표 클럭
            \return 거리시간(ns)
            */
            static sint64 GetClockPeriodNsec(id_clock_read from, id_clock_read to);

            /*!
            \brief 클럭에 시간더하기
            \param dest : 대상 클럭
            \param nsec : 더하는 시간(ns)
            */
            static void AddClockNsec(id_clock dest, sint64 nsec);

            /*!
            \brief 클럭의 세부정보
            \param clock : 대상 클럭
            \param nsec : 1초미만의 나노초(0 ~ 999,999,999)
            \param sec : 1분미만의 초(0 ~ 59)
            \param min : 1시간미만의 분(0 ~ 59)
            \param hour : 1일미만의 시간(0 ~ 23)
            \param day : 1개월미만의 일수(1 ~ 31)
            \param month : 1년미만의 개월수(1 ~ 12)
            \param year : 연도(2014, 2015, 2016, ...)
            */
            static void GetClockDetail(id_clock clock, sint64* nsec,
                sint32* sec = nullptr, sint32* min = nullptr, sint32* hour = nullptr, sint32* day = nullptr, sint32* month = nullptr, sint32* year = nullptr);

			/*!
            \brief 옵션플래그 설정하기
            \param name : 플래그명
            \param flag : 플래그값
            */
			static void SetOptionFlag(chars name, bool flag);

			/*!
            \brief 옵션플래그 얻어오기
            \param name : 플래그명
            \return 플래그값(디폴트값은 false)
            */
			static bool GetOptionFlag(chars name);

			/*!
            \brief 옵션플래그 리스팅
            \return 존재하는 모든 플래그명들
            */
			static Strings GetOptionFlagNames();

			/*!
            \brief 옵션페이로드 설정하기
            \param name : 페이로드명
            \param data : 데이터주소
            */
			static void SetOptionPayload(chars name, payload data);

			/*!
            \brief 옵션페이로드 얻어오기
            \param name : 페이로드명
            \return 데이터주소(디폴트값은 nullptr)
            */
			static payload GetOptionPayload(chars name);

			/*!
            \brief 옵션페이로드 리스팅
            \return 존재하는 모든 페이로드명
            */
			static Strings GetOptionPayloadNames();
        };

        //! \brief 그래픽스지원
        class Graphics
        {
        public:
            /*!
            \brief 절단면지정
            \param x : 좌측위치(px)
            \param y : 상단위치(px)
            \param w : 가로길이(px)
            \param h : 세로길이(px)
            */
            static void SetScissor(float x, float y, float w, float h);

            /*!
            \brief 컬러지정
            \param r : 적색값
            \param g : 녹색값
            \param b : 청색값
            \param a : 알파값
            */
            static void SetColor(uint08 r, uint08 g, uint08 b, uint08 a);

            /*!
            \brief 폰트지정
            \param name : 폰트명
            \param size : 사이즈값
            */
            static void SetFont(chars name, float size);

            /*!
            \brief 줌지정
            \param zoom : 줌값
            */
            static void SetZoom(float zoom);

            /*!
            \brief 사각형출력
            \param x : 좌측위치(px)
            \param y : 상단위치(px)
            \param w : 가로길이(px)
            \param h : 세로길이(px)
            */
            static void FillRect(float x, float y, float w, float h);

			/*!
            \brief 폴리곤출력
            \param x : 좌측위치(px)
            \param y : 상단위치(px)
            \param p : 꼭지점들(px)
            */
			static void FillPolygon(float x, float y, Points p);

            /*!
            \brief 타원출력
            \param x : 좌측위치(px)
            \param y : 상단위치(px)
            \param w : 가로길이(px)
            \param h : 세로길이(px)
            */
            static void DrawCircle(float x, float y, float w, float h);

            /*!
            \brief 라인출력
            \param begin : 시작점(px)
            \param end : 끝점(px)
            \param thick : 두께(px)
            */
            static void DrawLine(const Point& begin, const Point& end, float thick);

            /*!
            \brief 베지어출력
            \param begin : 시작점(px)
            \param end : 끝점(px)
            \param thick : 두께(px)
            */
            static void DrawBezier(const Vector& begin, const Vector& end, float thick);

            /*!
            \brief 폴리라인출력
            \param x : 좌측위치(px)
            \param y : 상단위치(px)
            \param p : 꼭지점들(px)
            \param thick : 두께(px)
            */
            static void DrawPolyLine(float x, float y, Points p, float thick);

            /*!
            \brief 폴리베지어출력
            \param x : 좌측위치(px)
            \param y : 상단위치(px)
            \param p : 꼭지점들(px)
            \param thick : 두께(px)
            \param showfirst : 첫선 보임여부
            \param showlast : 끝선 보임여부
            */
            static void DrawPolyBezier(float x, float y, Points p, float thick, bool showfirst, bool showlast);

            /*!
            \brief 이미지 생성
            \param bitmap : 비트맵
            \param coloring : 컬러링 색상
			\param resizing_width : 리사이징용 가로길이(px)
			\param resizing_height : 리사이징용 세로길이(px)
            \return 이미지
            */
            static id_image CreateImage(id_bitmap_read bitmap,
				const Color& coloring = Color(Color::ColoringDefault), sint32 resizing_width = -1, sint32 resizing_height = -1);

			/*!
            \brief 이미지 가로길이 얻기
            \param image : 이미지
            \return 이미지 가로길이
            */
			static sint32 GetImageWidth(id_image_read image);

			/*!
            \brief 이미지 세로길이 얻기
            \param image : 이미지
            \return 이미지 세로길이
            */
			static sint32 GetImageHeight(id_image_read image);

            /*!
            \brief 이미지 해제
            \param image : 이미지
            */
            static void RemoveImage(id_image image);

            /*!
            \brief 이미지출력
            \param image : 출력할 이미지
            \param ix : 이미지의 좌측위치(px)
            \param iy : 이미지의 상단위치(px)
            \param iw : 이미지의 가로길이(px)
            \param ih : 이미지의 세로길이(px)
            \param x : 좌측위치(px)
            \param y : 상단위치(px)
            \param w : 가로길이(px)
            \param h : 세로길이(px)
            */
            static void DrawImage(id_image_read image, float ix, float iy, float iw, float ih, float x, float y, float w, float h);

            /*!
            \brief 문자열출력
            \param x : 좌측위치(px)
            \param y : 상단위치(px)
            \param w : 가로길이(px)
            \param h : 세로길이(px)
            \param string : 문자열
            \param align : 정렬
            \param elide : 생략기호(...) 방향
            \return 실제로 생략이 되었는지의 여부
            */
            static bool DrawString(float x, float y, float w, float h, chars string, UIFontAlign align, UIFontElide elide = UIFE_None);

            /*!
            \brief 문자열출력(UTF16)
            \param x : 좌측위치(px)
            \param y : 상단위치(px)
            \param w : 가로길이(px)
            \param h : 세로길이(px)
            \param string : 문자열
            \param align : 정렬
            \param elide : 생략기호(...) 방향
            \return 실제로 생략이 되었는지의 여부
            */
            static bool DrawStringW(float x, float y, float w, float h, wchars string, UIFontAlign align, UIFontElide elide = UIFE_None);

            /*!
            \brief 폰트의 가로길이 얻기
            \param string : 문자열
            \return 가로길이(px)
            */
            static sint32 GetStringWidth(chars string);

            /*!
            \brief 폰트의 가로길이 얻기(UTF16)
            \param string : 문자열
            \return 가로길이(px)
            */
            static sint32 GetStringWidthW(wchars string);

            /*!
            \brief 폰트의 세로길이 얻기
            \return 세로길이(px)
            */
            static sint32 GetStringHeight();

            /*!
            \brief 폰트의 Ascent위치 얻기
            \return Ascent위치(px)
            */
            static sint32 GetStringAscent();

            /*!
            \brief GL드로잉 시작
            */
            static void BeginGL();

            /*!
            \brief GL드로잉 끝
            */
            static void EndGL();
        };

        //! \brief 파일지원
        class File
        {
        public:
            typedef void (*SearchCB)(chars, payload);

        public:
            /*!
            \brief 파일존재여부 얻기
            \param filename : 파일명
            \return 파일존재여부
            */
            static bool Exist(chars filename);

            /*!
            \brief 폴더존재여부 얻기
            \param dirname : 폴더명
            \return 폴더존재여부
            */
            static bool ExistForDir(chars dirname);

            /*!
            \brief 파일열기(읽기전용)
            \param filename : 파일명
            \return 파일ID
            \see Close
            */
            static id_file_read OpenForRead(chars filename);

            /*!
            \brief 파일열기(쓰기전용)
            \param filename : 파일명
            \param autocreatedir : 폴더가 없을 경우 자동으로 생성
            \return 파일ID
            \see Close
            */
            static id_file OpenForWrite(chars filename, bool autocreatedir = false);

            /*!
            \brief 파일닫기
            \param file : 파일ID
            \see OpenForRead, OpenForWrite
            */
            static void Close(id_file_read file);

            /*!
            \brief 파일사이즈 얻기
            \param file : 파일ID
            \return 파일사이즈
            */
            static const sint32 Size(id_file_read file);

            /*!
            \brief 파일읽기
            \param file : 파일ID
            \param data : 저장할 데이터
            \param size : 데이터의 길이(byte)
			\return 읽은 파일사이즈
            \see OpenForRead
            */
            static const sint32 Read(id_file_read file, uint08* data, const sint32 size);

			/*!
            \brief 파일에서 한줄읽기
            \param file : 파일ID
            \param text : 저장할 텍스트
            \param size : 텍스트의 길이(byte, 널문자포함)
			\return 읽은 파일사이즈
            \see OpenForRead
            */
			static const sint32 ReadLine(id_file_read file, char* text, const sint32 size);

            /*!
            \brief 파일쓰기
            \param file : 파일ID
            \param data : 불러올 데이터
            \param size : 데이터의 길이(byte)
			\return 쓴 파일사이즈
            \see OpenForWrite
            */
            static const sint32 Write(id_file file, bytes data, const sint32 size);

            /*!
            \brief 파일위치지정(SEEK_SET)
            \param file : 파일ID
            \param focus : 위치할 곳(byte)
            \see OpenForRead, OpenForWrite, Focus
            */
            static void Seek(id_file_read file, const sint32 focus);

			/*!
            \brief 파일포커스
            \param file : 파일ID
            \see Seek
            */
            static const sint32 Focus(id_file_read file);

            /*!
            \brief 대상폴더내 파일/폴더 검색
            \param dirname : 대상폴더명
            \param cb : 콜백함수
            \param data : 콜백함수에 전달할 데이터
            \param needfullpath : 콜백함수에 풀패스를 전달할지의 여부
            \return 검색된 수량
            */
            static sint32 Search(chars dirname, SearchCB cb, payload data, bool needfullpath);

            /*!
            \brief 아이템의 속성값 조사
            \param itemname : 아이템명(파일 또는 폴더)
            \param size : 파일사이즈 얻기
            \param ctime : 파일생성시간 얻기
            \param atime : 파일접근시간 얻기
            \param mtime : 파일수정시간 얻기
            \return 속성값
            */
            static uint32 GetAttributes(wchars itemname, uint64* size = nullptr, uint64* ctime = nullptr, uint64* atime = nullptr, uint64* mtime = nullptr);

            /*!
            \brief 아이템의 풀패스명 조사
            \param itemname : 아이템명(파일 또는 폴더)
            \return 풀패스명
            */
            static WString GetFullPath(wchars itemname);

            /*!
            \brief 아이템의 폴더명 조사
            \param itemname : 아이템명(파일 또는 폴더)
            \param badslash : 잘못된 슬래시기호
            \param goodslash : 정상적인 슬래시기호
            \return 폴더명
            */
            static WString GetDirName(wchars itemname, wchar_t badslash = L'\\', wchar_t goodslash = L'/');

            /*!
            \brief 아이템의 단축명 조사
            \param itemname : 아이템명(파일 또는 폴더)
            \return 단축명
            */
            static WString GetShortName(wchars itemname);

            /*!
            \brief 작업드라이브 코드명얻기
            \return 코드명(1~26)
            */
            static sint32 GetDriveCode();

            /*!
            \brief 아이템 존재확인
            \param itemname : 확인할 아이템명(파일 또는 폴더)
            \return 성공여부
            */
            static bool CanAccess(wchars itemname);

            /*!
            \brief 아이템 쓰기확인
            \param itemname : 확인할 아이템명(파일 또는 폴더)
            \return 성공여부
            */
            static bool CanWritable(wchars itemname);

            /*!
            \brief 아이템 삭제
            \param itemname : 삭제할 아이템명(파일 또는 폴더)
            \return 성공여부
            */
            static bool Remove(wchars itemname);

            /*!
            \brief 아이템 이름변경
            \param existing_itemname : 변경될 아이템명(파일 또는 폴더)
            \param new_itemname : 변경할 아이템명(파일 또는 폴더)
            \return 성공여부
            */
            static bool Rename(wchars existing_itemname, wchars new_itemname);

            /*!
            \brief 신규아이템 이름생성
            \param format : 아이템명 포맷(부여될 자리는 X로 표식)
            \param length : format의 길이
            \return 성공여부
            */
            static bool Tempname(char* format, sint32 length);

            /*!
            \brief 폴더 생성
            \param dirname : 생성할 폴더명
            \return 성공여부
            */
            static bool CreateDir(wchars dirname);

            /*!
            \brief 폴더 삭제
            \param dirname : 삭제할 폴더명
            \return 성공여부
            */
            static bool RemoveDir(wchars dirname);

            /*!
            \brief FD타입 파일열기
            \param filename : 파일명
            \param writable : 쓰기가 가능
            \param append : 파일끝에 추가
            \param exclusive : 기존 파일이 없을때만 가능
            \param truncate : 파일사이즈를 0으로 초기화
            \return 파일FD
            \see FDOpenRetain, FDClose
            */
            static sint32 FDOpen(wchars filename, bool writable, bool append, bool exclusive, bool truncate);

            /*!
            \brief FD타입 파일열기용 레퍼런스증가
            \param fd : 파일FD
            \see FDOpen, FDClose
            */
            static void FDOpenRetain(sint32 fd);

            /*!
            \brief FD타입 파일닫기
            \param fd : 파일FD
            \return 성공여부
            \see FDOpen, FDOpenRetain
            */
            static bool FDClose(sint32 fd);

            /*!
            \brief FD타입 파일읽기
            \param fd : 파일FD
            \param data : 읽어올 데이터
            \param size : 데이터의 길이
            \return 실제로 읽어온 길이
            */
            static sint64 FDRead(sint32 fd, void* data, sint64 size);

            /*!
            \brief FD타입 파일쓰기
            \param fd : 파일FD
            \param data : 쓸 데이터
            \param size : 데이터의 길이
            \return 실제로 쓴 길이
            */
            static sint64 FDWrite(sint32 fd, const void* data, sint64 size);

            /*!
            \brief FD타입 포커싱
            \param fd : 파일FD
            \param offset : origin으로부터의 상대위치
            \param origin : 0-파일시작, 1-마지막포커스, 2-파일끝
            \return 마지막포커스
            */
            static sint64 FDSeek(sint32 fd, sint64 offset, sint32 origin);

            /*!
            \brief FD타입 리사이징
            \param fd : 파일FD
            \param size : 변경할 파일사이즈
            \return 성공여부
            */
            static bool FDResize(sint32 fd, sint64 size);

            /*!
            \brief FD타입 메모리매핑 설정
            \param fd : 파일FD
            \param offset : 매핑할 시작위치
            \param size : 매핑할 사이즈
            \param readonly : 읽기전용
            \return 매핑된 메모리주소
            \see FDUnmap
            */
            static void* FDMap(sint32 fd, sint64 offset, sint64 size, bool readonly);

            /*!
            \brief FD타입 메모리매핑 해제
            \param map : 매핑된 메모리주소
            \return 성공여부
            \see FDMap
            */
            static bool FDUnmap(const void* map);

            /*!
            \brief FD타입 속성조사
            \param fd : 파일FD
            \param size : 파일사이즈
            \param ctime : 생성시간
            \param atime : 접근시간
            \param mtime : 수정시간
            \return 파일속성값
            */
            static uint32 FDAttributes(sint32 fd, uint64* size = nullptr, uint64* ctime = nullptr, uint64* atime = nullptr, uint64* mtime = nullptr);

            /*!
            \brief 원본(읽기전용) 어셋폴더 경로재정의
            \param dirname : 재정의할 폴더명
            */
            static void ResetAssetsRoot(chars dirname);

            /*!
            \brief 사본(읽기/쓰기) 어셋폴더 경로재정의
            \param dirname : 재정의할 폴더명
            */
            static void ResetAssetsRemRoot(chars dirname);

            /*!
            \brief 원본(읽기전용) 어셋폴더 경로얻기
            \return 경로
            */
            static const String& RootForAssets();

            /*!
            \brief 사본(읽기/쓰기) 어셋폴더 경로얻기
            \return 경로
            */
            static const String& RootForAssetsRem();

            /*!
            \brief 데이터전용(읽기/쓰기) 어셋폴더 경로얻기
            \return 경로
            */
            static const String& RootForData();
        };

        //! \brief 소켓지원
        class Socket
        {
        public:
            /*!
            \brief 소켓열기(TCP)
            \return 소켓ID
            \see Close
            */
            static id_socket OpenForTcp();

            /*!
            \brief 소켓열기(UDP)
            \return 소켓ID
            \see Close
            */
            static id_socket OpenForUdp();

            /*!
            \brief 소켓닫기
            \param socket : 소켓ID
            \param timeout : 타임아웃
            \return 성공여부
            \see OpenForTcp, OpenForUdp
            */
            static bool Close(id_socket socket, sint32 timeout = 3000);

            /*!
            \brief 서버와 연결
            \param socket : 소켓ID
            \param domain : 도메인명(IP가능)
            \param port : 포트번호
            \param timeout : 타임아웃
            \return 성공여부
            \see Disconnect
            */
            static bool Connect(id_socket socket, chars domain, uint16 port, sint32 timeout = 3000);

            /*!
            \brief 서버와 연결해제
            \param socket : 소켓ID
            \param timeout : 타임아웃
            \return 성공여부
            \see Connect
            */
            static bool Disconnect(id_socket socket, sint32 timeout = 3000);

            /*!
            \brief 대기중인 읽기용 버퍼조사
            \param socket : 소켓ID
            \return 읽을 수 있는 길이
            \see Recv
            */
            static sint32 RecvAvailable(id_socket socket);

            /*!
            \brief 소켓읽기
            \param socket : 소켓ID
            \param data : 읽어올 데이터
            \param size : 데이터의 길이
            \param timeout : 타임아웃
            \return 실제로 읽어온 길이
            \see RecvAvailable
            */
            static sint32 Recv(id_socket socket, uint08* data, sint32 size, sint32 timeout = 3000);

            /*!
            \brief 소켓쓰기
            \param socket : 소켓ID
            \param data : 쓸 데이터
            \param size : 데이터의 길이
            \param timeout : 타임아웃
            \return 실제로 쓴 길이
            */
            static sint32 Send(id_socket socket, bytes data, sint32 size, sint32 timeout = 3000);

            /*!
            \brief gethostbyname 기능제공
            \param name : 호스트명
            \return hostent구조체
            */
            static void* GetHostByName(chars name);

            /*!
            \brief getservbyname 기능제공
            \param name : 서비스명
            \param proto : 프로토콜명
            \return servent구조체
            */
            static void* GetServByName(chars name, chars proto);
        };

		//! \brief 서버지원
        class Server
		{
		public:
			/*!
			\brief 서버ID 할당
			\param sizefield : 사이즈필드 사용여부
			\return 서버ID(nullptr은 실패)
			\see Release, Listen
			*/
			static id_server Create(bool sizefield);

			/*!
			\brief 서버ID 반환
			\param server : 서버ID
			\see Create
			*/
			static void Release(id_server server);

			/*!
			\brief 서비스개시
			\param server : 서버ID
			\param port : 포트번호
			\return 성공여부
			\see Create, Release
			*/
			static bool Listen(id_server server, uint16 port);

			/*!
			\brief 수신된 패킷확인 및 포커싱
			\param server : 서버ID
			\return 수신여부
			\see GetPacketType, GetPacketPeerID, GetPacketBuffer
			*/
			static bool TryNextPacket(id_server server);

			/*!
			\brief 포커싱된 패킷종류 반환
			\param server : 서버ID
			\return 패킷종류
			\see TryNextPacket
			*/
			static packettype GetPacketType(id_server server);

			/*!
			\brief 포커싱된 패킷의 송신자ID 반환
			\param server : 서버ID
			\return 송신자ID
			\see TryNextPacket
			*/
			static sint32 GetPacketPeerID(id_server server);

			/*!
			\brief 포커싱된 패킷버퍼 반환
			\param server : 서버ID
			\param getsize : 버퍼의 길이를 요청
			\return 패킷버퍼
			\see TryNextPacket
			*/
			static bytes GetPacketBuffer(id_server server, sint32* getsize = nullptr);

			/*!
			\brief 특정 송신자에게 데이터전달
			\param server : 서버ID
			\param peerid : 송신자ID
			\param buffer : 데이터버퍼
			\param buffersize : 버퍼의 길이
			\return 성공여부
			*/
			static bool SendToPeer(id_server server, sint32 peerid, const void* buffer, sint32 buffersize);
		};

		//! \brief 시리얼통신지원
        class Serial
		{
		public:
			#define BLIK_SERIAL_ERROR_DISCONNECTED (-0x10000) // 접속해제, 해제사유코드
			#define BLIK_SERIAL_ERROR_BROKEN_FORMAT (-0x20000) // 파싱포맷의 문법오류<포맷스트링내 위치> (휴먼에러)
			#define BLIK_SERIAL_ERROR_SHORT_STREAM (-0x30000) // 읽기시 확보된 스트림부족<처리못한 가변인수의 순번> (휴먼에러)

			/*!
            \brief 시리얼포트이름 리스팅
			\param spec : json형태로 시리얼포트들의 스펙을 받음(선택사항)
            \return 존재하는 모든 시리얼포트이름
            */
			static Strings GetAllNames(String* spec = nullptr);

			/*!
			\brief 시리얼통신ID 할당
            \param name : 시리얼포트이름(없으면 첫번째 시리얼포트찾음)
			\param dec : 읽기시 
			\param enc :
			\return 시리얼통신ID(nullptr은 실패)
			\see Close
			*/
			static id_serial Open(chars name = "", SerialDecodeCB dec = nullptr, SerialEncodeCB enc = nullptr);

			/*!
			\brief 시리얼통신ID 반환
			\param serial : 시리얼통신ID
			\see Open
			*/
			static void Close(id_serial serial);

			/*!
			\brief 현재 접속상황
			\param serial : 시리얼통신ID
			\return 접속여부
			*/
			static bool Connected(id_serial serial);

			/*!
			\brief 읽기스트림으로 청크단위 수신하기(등록된 디코더가 없다면 불특정길이)
			\param serial : 시리얼통신ID
			\param gettype : 디코더에 의해 부여된 패킷타입ID
			\return 수신여부
			*/
			static bool ReadReady(id_serial serial, sint32* gettype = nullptr);

			/*!
			\brief 현재 읽기스트림의 사이즈얻기
			\param serial : 시리얼통신ID
			\return 대기중인 읽기스트림의 바이트길이(에러시, BLIK_SERIAL_ERROR계열)
			*/
			static sint32 ReadAvailable(id_serial serial);

			/*!
			\brief 읽기스트림에서 파싱포맷에 의해 읽기
			\param serial : 시리얼통신ID
			\param format : 파싱포맷(ex: "[be] #4 / #2 #1 #4:s2 [le] #4:skip, #1=n #n:str", 빈칸/콤마/슬래시등은 주석)
			\param ... : 포맷으로부터 입력받을 가변인수들(ex: &a, &b, &c)
			\return 읽은후 남은 읽기스트림의 바이트길이(에러시, BLIK_SERIAL_ERROR계열)
			*/
			static sint32 Read(id_serial serial, chars format, ...);

			/*!
			\brief 쓰기스트림에 파싱포맷에 의해 쓰기
			\param serial : 시리얼통신ID
			\param format : 파싱포맷(ex: "[be] #4=12 #2=0x00 #1 #4:s2 [le] #1:s4 #n:str", 빈칸/콤마/슬래시등은 주석)
			\param ... : 포맷으로 출력할 가변인수들(ex: a, b, c)
			\return 모여진 쓰기스트림의 바이트길이(에러시, BLIK_SERIAL_ERROR계열)
			*/
			static sint32 Write(id_serial serial, chars format, ...);

			/*!
			\brief 쓰기스트림을 송신하기
			\param serial : 시리얼통신ID
			\param type : 인코더에 전달될 패킷타입ID
			*/
			static void WriteFlush(id_serial serial, sint32 type = 0);
		};

        //! \brief 카메라지원
        class Camera
        {
        public:
			/*!
            \brief 카메라이름 리스팅
			\param spec : json형태로 카메라들의 스펙을 받음(선택사항)
            \return 존재하는 모든 카메라이름
            */
			static Strings GetAllNames(String* spec = nullptr);

			/*!
			\brief 카메라ID 할당
            \param name : 카메라이름(없으면 첫번째 카메라를 찾음)
            \param width : 가로해상도(-1이면 최대사이즈를 찾음)
            \param height : 세로해상도(-1이면 최대사이즈를 찾음)
			\return 카메라ID(nullptr은 실패)
			\see Close
			*/
            static id_camera Open(chars name = "", sint32 width = -1, sint32 height = -1);

			/*!
			\brief 카메라ID 반환
			\param camera : 카메라ID
			\see Open
			*/
			static void Close(id_camera camera);

            /*!
			\brief 카메라ID 복제
			\param camera : 카메라ID
            \return 복제된 카메라ID(nullptr은 실패)
            \see Open
			*/
            static id_camera Clone(id_camera camera);

			/*!
			\brief 촬영하기
            \param preview : 프리뷰식 촬영여부(해상도가 낮지만 촬영속도는 빠름)
			\param camera : 카메라ID
            \param needstop : 캡쳐직후 촬영을 중지할 여부
			*/
            static void Capture(id_camera camera, bool preview, bool needstop);

			/*!
            \brief 마지막 촬영한 이미지 얻기
			\param camera : 카메라ID
            \param maxwidth : 가로길이 최대값
            \param maxheight : 세로길이 최대값
			\return 촬영한 이미지(nullptr은 실패)
			*/
            static id_image_read LastCapturedImage(id_camera camera, sint32 maxwidth = -1, sint32 maxheight = -1);

			/*!
            \brief 마지막 촬영한 비트맵 얻기
			\param camera : 카메라ID
            \param vflip : 상하반전여부(일반적인 비트맵파일은 true)
			\return 촬영한 비트맵(nullptr은 실패)
			*/
			static id_bitmap_read LastCapturedBitmap(id_camera camera, bool vflip = true);

            /*!
            \brief 마지막 촬영한 이미지크기 얻기
            \param camera : 카메라ID
            \return 촬영한 이미지크기
            */
            static size64 LastCapturedSize(id_camera camera);

            /*!
            \brief 마지막 촬영한 시간 얻기
            \param camera : 카메라ID
            \return 촬영한 시간
            */
            static uint64 LastCapturedTimeMS(id_camera camera);

            /*!
            \brief 픽쳐방식으로 얻어온 총 촬영횟수
            \param camera : 카메라ID
            \return 촬영횟수
            */
            static sint32 TotalPictureShotCount(id_camera camera);

            /*!
            \brief 프리뷰방식으로 얻어온 총 촬영횟수
            \param camera : 카메라ID
            \return 촬영횟수
            */
            static sint32 TotalPreviewShotCount(id_camera camera);
		};

        //! \brief 마이크지원
        class Microphone
        {
        public:
            /*!
            \brief 마이크이름 리스팅
			\param spec : json형태로 마이크들의 스펙을 받음(선택사항)
            \return 존재하는 모든 마이크이름
            */
			static Strings GetAllNames(String* spec = nullptr);

            /*!
			\brief 마이크ID 할당
            \param name : 마이크이름(없으면 첫번째 마이크를 찾음)
            \param maxcount : 사운드큐의 최대크기(너무 크게 잡으면 비디오와의 싱크가 맞지 않음)
			\return 마이크ID(nullptr은 실패)
			\see Close
			*/
            static id_microphone Open(chars name = "", sint32 maxcount = 4);

            /*!
			\brief 마이크ID 반환
			\param microphone : 마이크ID
			\see Open
			*/
			static void Close(id_microphone microphone);

            /*!
			\brief BitRate정보 얻기
			\param microphone : 마이크ID
			\return BitRate정보
			*/
            static sint32 GetBitRate(id_microphone microphone);

            /*!
			\brief Channel정보 얻기
			\param microphone : 마이크ID
			\return Channel정보
			*/
            static sint32 GetChannel(id_microphone microphone);

            /*!
			\brief SampleRate정보 얻기
			\param microphone : 마이크ID
			\return SampleRate정보
			*/
            static sint32 GetSampleRate(id_microphone microphone);

            /*!
			\brief 적재된 사운드확인 및 포커싱
			\param microphone : 마이크ID
            \return 포키싱여부
			*/
            static bool TryNextSound(id_microphone microphone);

            /*!
			\brief 포커싱된 사운드데이터 반환
			\param microphone : 마이크ID
            \return 사운드데이터(audio/pcm)
            \see TryNextSound
			*/
            static bytes GetSoundData(id_microphone microphone);

            /*!
			\brief 포커싱된 사운드데이터의 크기 반환
			\param microphone : 마이크ID
            \return 사운드데이터의 크기(바이트단위)
            \see TryNextSound
			*/
            static sint32 GetSoundDataLength(id_microphone microphone);
        };
    };
}
