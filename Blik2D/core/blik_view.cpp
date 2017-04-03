#include <blik.hpp>
#include "blik_view.hpp"

namespace BLIK
{
    View::View()
    {
    }

    View::~View()
    {
    }

    View* View::Creator(chars viewclass)
    {
        BLIK_ASSERT("뷰생성기가 없습니다\nPlatform::SetViewCreator를 이용하세요", false);
        return new View();
    }

    const Map<h_view>* View::Search(chars viewclass, SearchCommand command, h_view view)
    {
        typedef Map<h_view> ViewMap;
        ViewMap& AllViews = *BLIK_STORAGE_SYS(ViewMap);
        Map<ViewMap>& ViewsInClass = *BLIK_STORAGE_SYS(Map<ViewMap>);

        if(command == SC_Search)
        {
            if(viewclass)
                return &ViewsInClass(viewclass);
            else return &AllViews;
        }

        const void* ViewPtr = view.get();
        const uint64 CurKey = PtrToUint64(ViewPtr);
        BLIK_ASSERT("사용할 수 없는 Key입니다", CurKey);
        chars ViewName = (viewclass && *viewclass)? viewclass : "_defaultview_";
        switch(command)
        {
        case SC_Create:
            {
                BLIK_ASSERT("이미 생성된 뷰가 존재합니다", !AllViews.Access(CurKey));
                AllViews[CurKey] = view;
                ViewsInClass(ViewName)[CurKey] = view;
            }
            break;
        case SC_Destroy:
            {
                BLIK_ASSERT("해제할 뷰가 존재하지 않습니다", AllViews.Access(CurKey));
                AllViews.Remove(CurKey);
                ViewsInClass(ViewName).Remove(CurKey);
            }
            break;
        }
        return nullptr;
    }

    h_view View::SetView(h_view view)
    {
        return h_view::null();
    }

    bool View::IsNative()
    {
        return false;
    }

    void* View::GetClass()
    {
        return nullptr;
    }

    void View::SendNotify(chars sender, chars topic, id_share in, id_cloned_share* out)
    {
    }

    void View::SetCallback(UpdaterCB cb, payload data)
    {
    }

    void View::OnCreate()
    {
    }

    bool View::OnCanQuit()
    {
        return true;
    }

    void View::OnDestroy()
    {
    }

    void View::OnSize(sint32 w, sint32 h)
    {
    }

    void View::OnTick()
    {
    }

    void View::OnRender(sint32 width, sint32 height, float l, float t, float r, float b)
    {
    }

    void View::OnTouch(TouchType type, sint32 id, sint32 x, sint32 y)
    {
    }
}
