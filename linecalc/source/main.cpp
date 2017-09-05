#include <blik.hpp>
#include <platform/blik_platform.hpp>
#include <service/blik_zay.hpp>

#include <r.hpp>

void PlatformInit()
{
    Platform::InitForGL();
    Platform::SetViewCreator(ZayView::Creator);

    Platform::SetWindowName("Line Calculator");
    Platform::SetWindowView("linecalcView");

    String InfoString = String::FromFile("windowinfo.json");
    if(0 < InfoString.Length())
    {
        Context Info(ST_Json, SO_OnlyReference, InfoString, InfoString.Length());
        Platform::SetWindowPos(Info("x").GetInt(0), Info("y").GetInt(0));
        Platform::SetWindowSize(Info("w").GetInt(640), Info("h").GetInt(480));
    }
    else Platform::SetWindowSize(640, 480);

    String AtlasInfoString = String::FromFile("atlasinfo.json");
    Context AtlasInfo(ST_Json, SO_OnlyReference, AtlasInfoString, AtlasInfoString.Length());
    R::SetAtlasDir("image");
    R::AddAtlas("ui_atlaskey.png", "atlas.png", AtlasInfo);
    if(R::IsAtlasUpdated())
        R::RebuildAll();

    Platform::AddWindowProcedure(WE_Tick,
        [](payload data)->void
        {
            static uint64 LastUpdateCheckTime = Platform::Utility::CurrentTimeMsec();
            uint64 CurUpdateCheckTime = Platform::Utility::CurrentTimeMsec();
            if(LastUpdateCheckTime + 1000 < CurUpdateCheckTime)
            {
                LastUpdateCheckTime = CurUpdateCheckTime;
                if(R::IsAtlasUpdated())
                {
                    R::RebuildAll();
                    Platform::UpdateAllViews();
                }
            }
        });
}

void PlatformQuit()
{
    rect128 WindowRect;
    Platform::GetWindowRect(WindowRect);

    Context Info;
    Info.At("x").Set(String::FromInteger(WindowRect.l));
    Info.At("y").Set(String::FromInteger(WindowRect.t));
    Info.At("w").Set(String::FromInteger(WindowRect.r - WindowRect.l));
    Info.At("h").Set(String::FromInteger(WindowRect.b - WindowRect.t));
    Info.SaveJson().ToFile("windowinfo.json");

    Context AtlasInfo;
    R::SaveAtlas(AtlasInfo);
    AtlasInfo.SaveJson().ToFile("atlasinfo.json");
}
