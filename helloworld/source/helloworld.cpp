#include <blik.hpp>
#include "helloworld.hpp"
#include <r.hpp>

BLIK_DECLARE_VIEW_CLASS("HWView", HWData)

BLIK_VIEW_API OnCommand(CommandType type, chars topic, id_share in, id_cloned_share* out)
{
    if(type == CT_Tick)
    {
        // 맵갱신을 인지하여 리로드
        static uint64 LastUpdateCheckTime = Platform::Utility::CurrentTimeMS();
        uint64 CurUpdateCheckTime = Platform::Utility::CurrentTimeMS();
        if(LastUpdateCheckTime + 1000 < CurUpdateCheckTime)
        {
            LastUpdateCheckTime = CurUpdateCheckTime;
            if(R::IsAtlasUpdated())
            {
                R::RebuildAll();
                m->invalidate();
            }
        }
    }
}

BLIK_VIEW_API OnNotify(chars sender, chars topic, id_share in, id_cloned_share* out)
{
}

BLIK_VIEW_API OnPanel(GestureType type, sint32 x, sint32 y)
{
}

BLIK_VIEW_API OnRender(ViewPanel& panel)
{
    BLIK_RGB(panel, 255, 255, 255)
        panel.fill();

    BLIK_RGBA(panel, 160, 128, 128, 16)
    {
        panel.icon(R("mark_img"), UIA_CenterMiddle);

        BLIK_RGBA(panel, 64, 64, 64, 255)
        BLIK_FONT(panel, 1.5, "arial black")
        BLIK_LTRB(panel, 0, 0, panel.w(), panel.h() / 3)
            panel.text(m->mMessage, UIFA_CenterMiddle, UIFE_Right);
    }

    BLIK_LTRB(panel, 0, panel.h() * 2 / 3, panel.w(), panel.h())
    BLIK_INNER_UI(panel, 30, "exit",
        BLIK_GESTURE_T(type)
        {
            if(type == GT_InReleased)
                Platform::Utility::ExitProgram();
        })
    {
        bool IsFocused = !!(panel.state("exit") & PS_Focused);
        bool IsPressed = !!(panel.state("exit") & PS_Pressed);

        BLIK_RGBA_IF(panel, 220, 220, 128, 128, IsFocused)
        if(panel.ninepatch(R("button_img")) == haschild_ok)
        {
            BLIK_CHILD_SCISSOR(panel)
            BLIK_FONT(panel, 3.0)
            BLIK_RGBA(panel, 0, 0, 0, 64)
            BLIK_RGBA_IF(panel, 255, 0, 0, 64, IsPressed)
                panel.text("Press to exit!", UIFA_CenterMiddle);
        }
    }
}

HWData::HWData()
{
    mMessage = "Hello World!";
}

HWData::~HWData()
{
}
