#pragma once
#include <blik.hpp>

namespace BLIK
{
    //! \brief 포맷-BMP
    class Bmp
    {
    public:
        typedef struct {uint32 size; sint16 param1; sint16 param2; uint32 offbits;} bitmapfile;
        typedef struct {uint32 size; sint32 width; sint32 height;
            sint16 planes; sint16 bitcount; uint32 compression; uint32 size_image;
            sint32 xpels_meter; sint32 ypels_meter; uint32 color_used; uint32 color_important;} bitmapinfo;
        typedef union {argb32 argb; struct {uint08 b; uint08 g; uint08 r; uint08 a;};} bitmappixel;

    public:
        static id_bitmap Create(sint32 bytesperpixel, sint32 width, sint32 height, sint16 param1 = 0, sint16 param2 = 0);
        static id_bitmap Clone(id_bitmap_read bitmap);
        static id_bitmap CloneFromBits(bytes bits, sint32 width, sint32 height, sint32 bitcount, bool flip);
        static id_bitmap Copy(id_bitmap_read bitmap, sint32 l, sint32 t, sint32 r, sint32 b);
        static void ChangeColor(id_bitmap bitmap, argb32 from, argb32 to);
        static void Remove(id_bitmap bitmap);

    public:
        static void MinTest(id_bitmap_read bitmap, rect128& recttest);
        static void MaxTest(id_bitmap_read bitmap, rect128& recttest);
        static void MaxTestWithBgColor(id_bitmap_read bitmap, rect128& recttest, argb32 bgcolor);

    public:
        static uint32 GetFileSizeWithoutBM(id_bitmap_read bitmap);
        static sint16 GetParam1(id_bitmap_read bitmap);
        static sint16 GetParam2(id_bitmap_read bitmap);
        static sint32 GetWidth(id_bitmap_read bitmap);
        static sint32 GetHeight(id_bitmap_read bitmap);
        static sint16 GetBitCount(id_bitmap_read bitmap);
        static uint32 GetImageSize(id_bitmap_read bitmap);
        static bytes GetBits(id_bitmap_read bitmap);
        static uint08* GetBits(id_bitmap bitmap);
    };
}
