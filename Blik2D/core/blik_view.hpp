#pragma once
#define BLIK_NOT_INCLUDE_FOR_INTELLIGENCE
#include <blik.hpp>

namespace BLIK
{
    BLIK_DECLARE_HANDLE(h_view);
    enum TouchType {TT_Null,
        // 일반
        TT_Moving, TT_Press, TT_Dragging, TT_Release,
        // 마우스휠
        TT_WheelUp, TT_WheelDown, TT_WheelPress, TT_WheelDragging, TT_WheelRelease,
        // 마우스확장(우클릭드래그)
        TT_ExtendPress, TT_ExtendDragging, TT_ExtendRelease,
        // 특수
        TT_ToolTip, TT_LongPress, TT_Render};
    enum SearchCommand {SC_Search, SC_Create, SC_Destroy};

    //! \brief 뷰지원
    class View
    {
    public:
        typedef View* (*CreatorCB)(chars);
        typedef void (*UpdaterCB)(payload, sint32);

    protected:
        View();
    public:
        virtual ~View();

    public:
        static View* Creator(chars viewclass);
        static const Map<h_view>* Search(chars viewclass, SearchCommand command, h_view view = h_view::null());

    public:
        virtual h_view SetView(h_view view);
        virtual bool IsNative();
        virtual void* GetClass();
        virtual void SendNotify(chars sender, chars topic, id_share in, id_cloned_share* out);
        virtual void SetCallback(UpdaterCB cb, payload data);

    public:
        virtual void OnCreate();
        virtual bool OnCanQuit();
        virtual void OnDestroy();
        virtual void OnSize(sint32 w, sint32 h);
        virtual void OnTick();
        virtual void OnRender(sint32 width, sint32 height, float l, float t, float r, float b);
        virtual void OnTouch(TouchType type, sint32 id, sint32 x, sint32 y);
        virtual void OnKey(sint32 code, chars text, bool pressed);
    };
}
