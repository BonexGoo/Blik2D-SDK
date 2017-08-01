#include <blik.hpp>
#include "linecalc.hpp"

#include <r.hpp>

ZAY_DECLARE_VIEW_CLASS("linecalcView", linecalcData)

ZAY_VIEW_API OnCommand(CommandType type, chars topic, id_share in, id_cloned_share* out)
{
    if(type == CT_Tick)
    {
        static uint64 LastUpdateCheckTime = Platform::Utility::CurrentTimeMsec();
        uint64 CurUpdateCheckTime = Platform::Utility::CurrentTimeMsec();
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

ZAY_VIEW_API OnNotify(chars sender, chars topic, id_share in, id_cloned_share* out)
{
}

ZAY_VIEW_API OnGesture(GestureType type, sint32 x, sint32 y)
{
}

ZAY_VIEW_API OnRender(ZayPanel& panel)
{
    ZAY_RGB(panel, 160, 192, 224)
        panel.fill();

    ZAY_LTRB(panel, 10, 10, panel.w() - 10, 10 + 26)
    {
        // 변수창
        ZAY_LTRB_UI(panel, 0, 0, 80, panel.h(), "Variable",
            ZAY_GESTURE_VNT(v, n, t)
            {
                if(t == GT_Pressed)
                {
                    auto& UIRect = v->rect(n);
                    String Text = m->CurVariable;
                    if(Platform::Popup::OpenEditTracker(Text,
                        UIET_String, UIRect.l, UIRect.t, UIRect.r, UIRect.b))
                        m->CurVariable = Text;
                }
            })
        {
            ZAY_RGB(panel, 255, 255, 192)
                panel.fill();
            ZAY_LTRB(panel, 5, 0, panel.w() - 5, panel.h())
            ZAY_RGB(panel, 0, 0, 0)
                panel.text(m->CurVariable, UIFA_LeftMiddle, UIFE_Right);
        }

        // 계산식창
        ZAY_LTRB_UI_SCISSOR(panel, 80 + 10, 0, panel.w() - (80 + 10) * 2, panel.h(), "Formula",
            ZAY_GESTURE_VNT(v, n, t)
            {
                if(t == GT_Pressed)
                {
                    auto& UIRect = v->rect(n);
                    String Text = m->CurFormula;
                    if(Platform::Popup::OpenEditTracker(Text,
                        UIET_String, UIRect.l, UIRect.t, UIRect.r, UIRect.b))
                        m->CurFormula = Text;
                }
            })
        {
            ZAY_RGB(panel, 255, 255, 255)
                panel.fill();
            ZAY_LTRB(panel, 5, 0, panel.w() - 5, panel.h())
            ZAY_RGB(panel, 0, 0, 0)
                panel.text(m->CurFormula, UIFA_LeftMiddle, UIFE_Right);
        }

        // 추가버튼
        ZAY_XYWH_UI(panel, panel.w() - (80 * 2 + 10), 0, 80, panel.h(), "Add",
            ZAY_GESTURE_VNT(v, n, t)
            {
                if(t == GT_InReleased)
                {
                    Solver& NewSolver = m->CollectedSolvers.AtAdding();
                    NewSolver.Parse(m->CurFormula);
                    NewSolver.Link("local", m->CurVariable);
                    NewSolver.Execute();
                    m->CurVariable.Empty();
                    m->CurFormula.Empty();
                }
            })
        {
            ZAY_RGBA(panel, 128, 255, 0, 128)
            ZAY_RGBA_IF(panel, 192, 192, 192, 128, panel.state("Add") & PS_Focused)
                panel.fill();
            ZAY_RGB(panel, 0, 0, 0)
            ZAY_RGB_IF(panel, 192, 64, 64, panel.state("Add") & (PS_Pressed | PS_Dragging))
                panel.text("Add", UIFA_CenterMiddle, UIFE_Right);
        }

        // 일괄업데이트버튼
        ZAY_XYWH_UI(panel, panel.w() - 80, 0, 80, panel.h(), "Update",
            ZAY_GESTURE_VNT(v, n, t)
            {
                if(t == GT_InReleased)
                {
                    for(sint32 i = 0, iend = m->CollectedSolvers.Count(); i < iend; ++i)
                        if(m->CollectedSolvers[i].need_execute())
                            m->CollectedSolvers.At(i).Execute();
                }
            })
        {
            ZAY_RGBA(panel, 0, 0, 255, 128)
            ZAY_RGBA_IF(panel, 192, 192, 192, 128, panel.state("Update") & PS_Focused)
                panel.fill();
            ZAY_RGB(panel, 0, 0, 0)
            ZAY_RGB_IF(panel, 192, 64, 64, panel.state("Update") & (PS_Pressed | PS_Dragging))
                panel.text("Update", UIFA_CenterMiddle, UIFE_Right);
        }
    }

    // 수집창
    ZAY_LTRB_SCISSOR(panel, 10, 10 + 26 + 10, panel.w() - 10, panel.h() - 10)
    {
        ZAY_RGB(panel, 160, 160, 160)
            panel.fill();

        // 계산식 리스트
        for(sint32 i = 0, iend = m->CollectedSolvers.Count(); i < iend; ++i)
        {
            ZAY_XYWH(panel, 10, 10 + (24 + 10) * i, panel.w() - 20, 24)
            {
                ZAY_RGBA(panel, 255, 255, 255, 128)
                    panel.fill();

                // 계산식
                ZAY_LTRB(panel, 0, 0, panel.w() - panel.h(), panel.h())
                {
                    String CurSolver = String::Format(" %s = %s ▶ %lf (%.2f%%)",
                        (chars) m->CollectedSolvers[i].linked_variable(),
                        (chars) m->CollectedSolvers[i].parsed_formula(),
                        (double) m->CollectedSolvers[i].result(),
                        m->CollectedSolvers[i].reliable() * 100);
                    ZAY_RGB(panel, 0, 0, 0)
                        panel.text(CurSolver, UIFA_LeftMiddle, UIFE_Right);
                }

                // 업데이트버튼
                const String UpdateName = String::Format("Update%d", i);
                ZAY_XYRR_UI(panel, panel.w() - panel.h() * 3 / 2 + 4, panel.h() / 2, 8, 8, UpdateName,
                    ZAY_GESTURE_VNT(v, n, t, i)
                    {
                        if(t == GT_InReleased)
                            m->CollectedSolvers.At(i).Execute();
                    })
                {
                    ZAY_RGBA_IF(panel, 0, 128, 255, 128, m->CollectedSolvers[i].need_execute())
                    ZAY_RGBA_IF(panel, 96, 96, 96, 128, !m->CollectedSolvers[i].need_execute())
                    ZAY_RGBA_IF(panel, 192, 192, 192, 128, panel.state(UpdateName) & PS_Focused)
                        panel.fill();
                    ZAY_RGB(panel, 0, 0, 0)
                    ZAY_RGB_IF(panel, 64, 64, 192, panel.state(UpdateName) & (PS_Pressed | PS_Dragging))
                        panel.text("□", UIFA_CenterMiddle, UIFE_Right);
                }

                // 삭제버튼
                const String DeleteName = String::Format("Delete%d", i);
                ZAY_XYRR_UI(panel, panel.w() - panel.h() * 1 / 2, panel.h() / 2, 8, 8, DeleteName,
                    ZAY_GESTURE_VNT(v, n, t, i)
                    {
                        if(t == GT_InReleased)
                            m->DeleteSolver(i);
                    })
                {
                    ZAY_RGBA(panel, 255, 0, 0, 128)
                    ZAY_RGBA_IF(panel, 192, 192, 192, 128, panel.state(DeleteName) & PS_Focused)
                        panel.fill();
                    ZAY_RGB(panel, 0, 0, 0)
                    ZAY_RGB_IF(panel, 192, 64, 64, panel.state(DeleteName) & (PS_Pressed | PS_Dragging))
                        panel.text("×", UIFA_CenterMiddle, UIFE_Right);
                }
            }
        }
    }
}

linecalcData::linecalcData()
{
}

linecalcData::~linecalcData()
{
}

void linecalcData::DeleteSolver(sint32 i)
{
    for(sint32 j = i, jend = CollectedSolvers.Count() - 1; j < jend; ++j)
        CollectedSolvers.At(j) = CollectedSolvers.At(j + 1);
    CollectedSolvers.SubtractionOne();
}
