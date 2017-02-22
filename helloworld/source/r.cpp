#include <blik.hpp>
#include "r.hpp"

#include <format/blik_bmp.hpp>
#include <format/blik_png.hpp>
#include <service/blik_imagebuilder.hpp>
#include <platform/blik_platform.hpp>

namespace BLIK
{
	static Map<Image> gImageMap;
    static String gAtlasDir;

    R::R(chars name)
    {
        mImage = gImageMap.Access(name);
        if(!mImage)
        {
            mImage = &gImageMap(name);
            if(!mImage->SetName(gAtlasDir + name).Load())
            {
                id_asset_read PngAsset = Asset::OpenForRead(gAtlasDir + "noimg.png");
                const sint32 PngSize = Asset::Size(PngAsset);
                buffer PngBuffer = Buffer::Alloc(BLIK_DBG PngSize);
                Asset::Read(PngAsset, (uint08*) PngBuffer, PngSize);
                Asset::Close(PngAsset);
                mImage->LoadBitmap(Png().ToBmp((bytes) PngBuffer, true));
                Buffer::Free(PngBuffer);
            }
        }
    }

    R::~R()
    {
    }

	class AtlasSet
	{
	public:
		AtlasSet() {FileSize = 0; ModifyTime = 0;}
		~AtlasSet() {}
		AtlasSet& operator=(const AtlasSet& rhs)
		{
			KeyFilename = rhs.KeyFilename;
			MapFilename = rhs.MapFilename;
			FileSize = rhs.FileSize;
			ModifyTime = rhs.ModifyTime;
			return *this;
		}
	public:
		String KeyFilename;
		String MapFilename;
		sint32 FileSize;
		sint32 ModifyTime;
	};
	static Array<AtlasSet> gAtlasSets;

    void R::SetAtlasDir(chars dirname)
    {
        if(*dirname == '\0') gAtlasDir = "";
        else gAtlasDir = String::Format("%s/", dirname);
    }

	void R::AddAtlas(chars key_filename, chars map_filename, const Property& prop)
	{
        gAtlasSets.AtAdding();
		gAtlasSets.At(-1).KeyFilename = key_filename;
		gAtlasSets.At(-1).MapFilename = map_filename;
        gAtlasSets.At(-1).FileSize = prop(map_filename)("filesize").GetInt(0);
		gAtlasSets.At(-1).ModifyTime = prop(map_filename)("modifytime").GetInt(0);
	}

    void R::SaveAtlas(Property& prop)
    {
        for(sint32 i = 0, iend = gAtlasSets.Count(); i < iend; ++i)
        {
            prop.At(gAtlasSets[i].MapFilename).At("filesize").Set(String::FromInteger(gAtlasSets[i].FileSize));
            prop.At(gAtlasSets[i].MapFilename).At("modifytime").Set(String::FromInteger(gAtlasSets[i].ModifyTime));
        }
    }

	bool R::IsAtlasUpdated()
	{
		for(sint32 i = 0, iend = gAtlasSets.Count(); i < iend; ++i)
		{
			String FullPath = Platform::File::RootForAssets() + gAtlasDir + gAtlasSets[i].MapFilename;
			uint64 FileSize = 0, ModifyTime = 0;
			if(Platform::File::GetAttributes(WString::FromChars(FullPath), &FileSize, nullptr, nullptr, &ModifyTime) != -1)
			if(gAtlasSets[i].FileSize != (FileSize & 0x7FFFFFFF) || gAtlasSets[i].ModifyTime != (ModifyTime & 0x7FFFFFFF))
				return true;
		}
		return false;
	}

	void R::RebuildAll()
	{
		gImageMap.Reset();
		ImageBuilder Builder;
		for(sint32 i = 0, iend = gAtlasSets.Count(); i < iend; ++i)
		{
			String FullPath = Platform::File::RootForAssets() + gAtlasDir + gAtlasSets[i].MapFilename;
			uint64 FileSize = 0, ModifyTime = 0;
			if(Platform::File::GetAttributes(WString::FromChars(FullPath), &FileSize, nullptr, nullptr, &ModifyTime) != -1)
			if(Builder.LoadAtlas(gAtlasDir + gAtlasSets[i].KeyFilename, gAtlasDir + gAtlasSets[i].MapFilename, 0 < i))
			{
				gAtlasSets.At(i).FileSize = (sint32) (FileSize & 0x7FFFFFFF);
				gAtlasSets.At(i).ModifyTime = (sint32) (ModifyTime & 0x7FFFFFFF);
			}
		}
		Builder.SaveSubImages(gAtlasDir);
	}
}
