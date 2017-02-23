#pragma once
#include <element/blik_image.hpp>

namespace BLIK
{
    //! \brief 빌더-이미지
    class ImageBuilder
    {
    public:
        ImageBuilder();
        ~ImageBuilder();
        ImageBuilder& operator=(const ImageBuilder&)
        {
            BLIK_ASSERT("해당 객체는 복사할 수 없습니다", false);
            return *this;
        }

    public:
        bool LoadAtlas(chars key_filename, chars map_filename, bool keep_collection = false);
        void SaveSubImages(chars pathname = "") const;

    public:
        static id_bitmap MakeBitmap(chars png_filename, id_assetpath_read assetpath = nullptr);
        static id_bitmap MakeTagBitmap(chars key_filename, chars tagname);

    private:
        Array<Image> m_subimages;
    };
}
