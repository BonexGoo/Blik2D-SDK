#include <blik.hpp>
#include <platform/blik_platform.hpp>
#include <r.hpp>

void PlatformInit()
{
    Platform::InitForGL();
    Platform::SetWindowName("Hello World");
    Platform::SetWindowView("HWView");

    String InfoString = String::FromFile("windowinfo.json");
    if(0 < InfoString.Length())
    {
        Property Info(ST_Json, SO_OnlyReference, InfoString, InfoString.Length());
        Platform::SetWindowPos(Info("x").GetInt(0), Info("y").GetInt(0));
        Platform::SetWindowSize(Info("w").GetInt(640), Info("h").GetInt(480));
    }
    else Platform::SetWindowSize(640, 480);

    String AtlasInfoString = String::FromFile("image/mapinfo.json");
    Property AtlasInfo(ST_Json, SO_OnlyReference, AtlasInfoString, AtlasInfoString.Length());
    R::SetAtlasDir("image");
    R::AddAtlas("ui_atlaskey.png", "map.png", AtlasInfo);
    if(R::IsAtlasUpdated())
        R::RebuildAll();
}

void PlatformQuit()
{
    rect128 WindowRect;
    Platform::GetWindowRect(WindowRect);

    Property Info;
    Info.At("x").Set(String::FromInteger(WindowRect.l));
    Info.At("y").Set(String::FromInteger(WindowRect.t));
    Info.At("w").Set(String::FromInteger(WindowRect.r - WindowRect.l));
    Info.At("h").Set(String::FromInteger(WindowRect.b - WindowRect.t));
    Info.SaveJson().ToFile("windowinfo.json");

    Property AtlasInfo;
    R::SaveAtlas(AtlasInfo);
    AtlasInfo.SaveJson().ToFile("image/mapinfo.json");
}
