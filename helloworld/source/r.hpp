#pragma once
#include <element/blik_image.hpp>

namespace BLIK
{
    class R
    {
	public:
        R(chars name);
        ~R();

	public:
        static void SetAtlasDir(chars dirname);
		static void AddAtlas(chars key_filename, chars map_filename, const Property& prop);
        static void SaveAtlas(Property& prop);
		static bool IsAtlasUpdated();
		static void RebuildAll();

    public:
        inline sint32 w() const {return (mImage)? mImage->GetWidth() : 0;}
        inline sint32 h() const {return (mImage)? mImage->GetHeight() : 0;}

    public:
        inline operator const Image&() const
        {
            BLIK_ASSERT("이미지가 로드되지 않았습니다", mImage);
            return *mImage;
        }
        inline const Image* operator->() const
        {
            BLIK_ASSERT("이미지가 로드되지 않았습니다", mImage);
            return mImage;
        }

    private:
        Image* mImage;
    };
}
