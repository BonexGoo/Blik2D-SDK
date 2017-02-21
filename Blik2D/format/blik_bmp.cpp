#include <blik.hpp>
#include "blik_bmp.hpp"

namespace BLIK
{
    id_bitmap Bmp::Create(sint32 bytesperpixel, sint32 width, sint32 height, sint16 param1, sint16 param2)
	{
		const sint32 BmpRow = (bytesperpixel * width + 3) & ~3;
		id_bitmap NewBitmap = (id_bitmap) Memory::Alloc(sizeof(bitmapfile) + sizeof(bitmapinfo) + BmpRow * height);
		bitmapfile* BmpFileHeader = (bitmapfile*) NewBitmap;
		BmpFileHeader->size = sizeof(bitmapfile) + sizeof(bitmapinfo) + BmpRow * height;
		BmpFileHeader->param1 = param1;
		BmpFileHeader->param2 = param2;
		BmpFileHeader->offbits = sizeof(bitmapfile) + sizeof(bitmapinfo);
		bitmapinfo* BmpInfoHeader = (bitmapinfo*) (((uint08*) NewBitmap) + sizeof(bitmapfile));
		BmpInfoHeader->size = sizeof(bitmapinfo);
		BmpInfoHeader->width = width;
		BmpInfoHeader->height = height;
		BmpInfoHeader->planes = 1;
		BmpInfoHeader->bitcount = 8 * bytesperpixel;
		BmpInfoHeader->compression = 0;
		BmpInfoHeader->size_image = BmpRow * height;
		BmpInfoHeader->xpels_meter = 3780;
		BmpInfoHeader->ypels_meter = 3780;
		BmpInfoHeader->color_used = 0;
		BmpInfoHeader->color_important = 0;
		return NewBitmap;
	}

    id_bitmap Bmp::Clone(id_bitmap_read bitmap)
    {
		const sint32 BitCount = GetBitCount(bitmap);
		if(BitCount == 24)
		{
			const sint32 Width = GetWidth(bitmap);
			const sint32 Height = GetHeight(bitmap);
			id_bitmap NewBitmap = Create(4, Width, Height, GetParam1(bitmap), GetParam2(bitmap));
			auto DstBits = (Bmp::bitmappixel*) GetBits(NewBitmap);
			bytes SrcBits = GetBits(bitmap);
			const sint32 SrcRow = (BitCount / 8 * Width + 3) & ~3;
			for(sint32 y = 0; y < Height; ++y)
			{
				bytes CurBits = &SrcBits[y * SrcRow];
				for(sint32 x = 0; x < Width; ++x)
				{
					DstBits->b = *(CurBits++);
					DstBits->g = *(CurBits++);
					DstBits->r = *(CurBits++);
					DstBits->a = 0xFF;
					DstBits++;
				}
			}
			return NewBitmap;
		}
		else if(BitCount == 32)
		{
			const uint32 BitmapSize = GetFileSizeWithoutBM(bitmap);
			id_bitmap NewBitmap = (id_bitmap) Memory::Alloc(BitmapSize);
			Memory::Copy((void*) NewBitmap, (const void*) bitmap, BitmapSize);
			return NewBitmap;
		}
		BLIK_ASSERT("본 함수가 지원하지 못하는 비트맵입니다", false);
		return nullptr;
    }

	id_bitmap Bmp::CloneFromBits(bytes bits, sint32 width, sint32 height, sint32 bitcount, bool flip)
	{
		if(bitcount == 24)
		{
			id_bitmap NewBitmap = Create(4, width, height);
			auto DstBits = (Bmp::bitmappixel*) GetBits(NewBitmap);
			const sint32 SrcRow = (bitcount / 8 * width + 3) & ~3;
			if(flip)
			{
				for(sint32 y = 0; y < height; ++y)
				{
					bytes CurBits = &bits[(height - 1 - y) * SrcRow];
					for(sint32 x = 0; x < width; ++x)
					{
						DstBits->b = *(CurBits++);
						DstBits->g = *(CurBits++);
						DstBits->r = *(CurBits++);
						DstBits->a = 0xFF;
						DstBits++;
					}
				}
			}
			else for(sint32 y = 0; y < height; ++y)
			{
				bytes CurBits = &bits[y * SrcRow];
				for(sint32 x = 0; x < width; ++x)
				{
					DstBits->b = *(CurBits++);
					DstBits->g = *(CurBits++);
					DstBits->r = *(CurBits++);
					DstBits->a = 0xFF;
					DstBits++;
				}
			}
			return NewBitmap;
		}
		else if(bitcount == 32)
		{
			id_bitmap NewBitmap = Create(4, width, height);
			auto DstBits = (Bmp::bitmappixel*) GetBits(NewBitmap);
			if(flip)
			{
				for(sint32 y = 0; y < height; ++y)
				{
					Memory::Copy(DstBits, &bits[(height - 1 - y) * sizeof(Bmp::bitmappixel) * width],
						sizeof(Bmp::bitmappixel) * width);
					DstBits += width;
				}
			}
			else Memory::Copy(DstBits, bits, sizeof(Bmp::bitmappixel) * width * height);
			return NewBitmap;
		}
		BLIK_ASSERT("본 함수가 지원하지 못하는 비트맵입니다", false);
		return nullptr;
	}

    id_bitmap Bmp::Copy(id_bitmap_read bitmap, sint32 l, sint32 t, sint32 r, sint32 b)
    {
        BLIK_ASSERT("본 함수는 32비트 비트맵만 지원합니다", GetBitCount(bitmap) == 32);
        const sint32 Width = GetWidth(bitmap);
        const sint32 Height = GetHeight(bitmap);
        const bitmappixel* Bits = (const bitmappixel*) GetBits(bitmap);

        const sint32 NewWidth = r - l;
        const sint32 NewHeight = b - t;
        id_bitmap NewBitmap = Create(4, NewWidth, NewHeight);
        bitmappixel* NewBits = (bitmappixel*) GetBits(NewBitmap);
        for(sint32 y = 0; y < NewHeight; ++y)
            Memory::Copy(&NewBits[(NewHeight - 1 - y) * NewWidth], &Bits[l + (Height - 1 - t - y) * Width], sizeof(bitmappixel) * NewWidth);
        return NewBitmap;
    }

	void Bmp::ChangeColor(id_bitmap bitmap, argb32 from, argb32 to)
	{
		const sint32 CurWidth = GetWidth(bitmap);
		const sint32 CurHeight = GetHeight(bitmap);
		bitmappixel* CurBits = (bitmappixel*) GetBits(bitmap);
		for(sint32 y = 0; y < CurHeight; ++y)
		for(sint32 x = 0; x < CurWidth; ++x)
		{
			bitmappixel& CurBit = CurBits[x + (CurHeight - 1 - y) * CurWidth];
            if(CurBit.argb == from) CurBit.argb = to;
		}
	}

    void Bmp::Remove(id_bitmap bitmap)
    {
        Memory::Free(bitmap);
    }

    void Bmp::MinTest(id_bitmap_read bitmap, rect128& recttest)
    {
        BLIK_ASSERT("본 함수는 32비트 비트맵만 지원합니다", GetBitCount(bitmap) == 32);
        const sint32 Width = GetWidth(bitmap);
        const sint32 Height = GetHeight(bitmap);
        const bitmappixel* Bits = (const bitmappixel*) GetBits(bitmap);

        bool AnyUpdated = true;
        while(AnyUpdated)
        {
            AnyUpdated = false;
            if(recttest.l + 1 < recttest.r)
            {
                bool EverybodyBg = true;
                const sint32 TestL = recttest.l;
                for(sint32 ly = Height - recttest.b, lyend = Height - recttest.t; ly < lyend; ++ly)
                    if(Bits[TestL + ly * Width].a)
                    {EverybodyBg = false; break;}
                if(EverybodyBg) {recttest.l++; AnyUpdated = true;}
            }
            if(recttest.t + 1 < recttest.b)
            {
                bool EverybodyBg = true;
                const sint32 TestT = Height - 1 - recttest.t;
                for(sint32 tx = recttest.l; tx < recttest.r; ++tx)
                    if(Bits[tx + TestT * Width].a)
                    {EverybodyBg = false; break;}
                if(EverybodyBg) {recttest.t++; AnyUpdated = true;}
            }
            if(recttest.l < recttest.r - 1)
            {
                bool EverybodyBg = true;
                const sint32 TestR = recttest.r - 1;
                for(sint32 ry = Height - recttest.b, ryend = Height - recttest.t; ry < ryend; ++ry)
                    if(Bits[TestR + ry * Width].a)
                    {EverybodyBg = false; break;}
                if(EverybodyBg) {recttest.r--; AnyUpdated = true;}
            }
            if(recttest.t < recttest.b - 1)
            {
                bool EverybodyBg = true;
                const sint32 TestB = Height - recttest.b;
                for(sint32 bx = recttest.l; bx < recttest.r; ++bx)
                    if(Bits[bx + TestB * Width].a)
                    {EverybodyBg = false; break;}
                if(EverybodyBg) {recttest.b--; AnyUpdated = true;}
            }
        }
    }

    void Bmp::MaxTest(id_bitmap_read bitmap, rect128& recttest)
    {
        BLIK_ASSERT("본 함수는 32비트 비트맵만 지원합니다", GetBitCount(bitmap) == 32);
        const sint32 Width = GetWidth(bitmap);
        const sint32 Height = GetHeight(bitmap);
        const bitmappixel* Bits = (const bitmappixel*) GetBits(bitmap);

        bool AnyUpdated = true;
        while(AnyUpdated)
        {
            AnyUpdated = false;
            if(0 < recttest.l)
            {
                const sint32 TestL = recttest.l - 1;
                for(sint32 ly = Height - recttest.b, lyend = Height - recttest.t; ly < lyend; ++ly)
                    if(Bits[TestL + ly * Width].a)
                    {recttest.l--; AnyUpdated = true; break;}
            }
            if(0 < recttest.t)
            {
                const sint32 TestT = Height - recttest.t;
                for(sint32 tx = recttest.l; tx < recttest.r; ++tx)
                    if(Bits[tx + TestT * Width].a)
                    {recttest.t--; AnyUpdated = true; break;}
            }
            if(recttest.r < Width)
            {
                const sint32 TestR = recttest.r;
                for(sint32 ry = Height - recttest.b, ryend = Height - recttest.t; ry < ryend; ++ry)
                    if(Bits[TestR + ry * Width].a)
                    {recttest.r++; AnyUpdated = true; break;}
            }
            if(recttest.b < Height)
            {
                const sint32 TestB = Height - 1 - recttest.b;
                for(sint32 bx = recttest.l; bx < recttest.r; ++bx)
                    if(Bits[bx + TestB * Width].a)
                    {recttest.b++; AnyUpdated = true; break;}
            }
        }
    }

    void Bmp::MaxTestWithBgColor(id_bitmap_read bitmap, rect128& recttest, argb32 bgcolor)
    {
        BLIK_ASSERT("본 함수는 32비트 비트맵만 지원합니다", GetBitCount(bitmap) == 32);
        const sint32 Width = GetWidth(bitmap);
        const sint32 Height = GetHeight(bitmap);
        const bitmappixel* Bits = (const bitmappixel*) GetBits(bitmap);

        bool AnyUpdated = true;
        while(AnyUpdated)
        {
            AnyUpdated = false;
            if(0 < recttest.l)
            {
                const sint32 TestL = recttest.l - 1;
                for(sint32 ly = Height - recttest.b, lyend = Height - recttest.t; ly < lyend; ++ly)
                {
                    const Bmp::bitmappixel& CurColor = Bits[TestL + ly * Width];
                    if(CurColor.a && CurColor.argb != bgcolor)
                    {recttest.l--; AnyUpdated = true; break;}
                }
            }
            if(0 < recttest.t)
            {
                const sint32 TestT = Height - recttest.t;
                for(sint32 tx = recttest.l; tx < recttest.r; ++tx)
                {
                    const Bmp::bitmappixel& CurColor = Bits[tx + TestT * Width];
                    if(CurColor.a && CurColor.argb != bgcolor)
                    {recttest.t--; AnyUpdated = true; break;}
                }
            }
            if(recttest.r < Width)
            {
                const sint32 TestR = recttest.r;
                for(sint32 ry = Height - recttest.b, ryend = Height - recttest.t; ry < ryend; ++ry)
                {
                    const Bmp::bitmappixel& CurColor = Bits[TestR + ry * Width];
                    if(CurColor.a && CurColor.argb != bgcolor)
                    {recttest.r++; AnyUpdated = true; break;}
                }
            }
            if(recttest.b < Height)
            {
                const sint32 TestB = Height - 1 - recttest.b;
                for(sint32 bx = recttest.l; bx < recttest.r; ++bx)
                {
                    const Bmp::bitmappixel& CurColor = Bits[bx + TestB * Width];
                    if(CurColor.a && CurColor.argb != bgcolor)
                    {recttest.b++; AnyUpdated = true; break;}
                }
            }
        }
    }

    uint32 Bmp::GetFileSizeWithoutBM(id_bitmap_read bitmap)
    {
        return ((const bitmapfile*) bitmap)->size;
    }

	sint16 Bmp::GetParam1(id_bitmap_read bitmap)
	{
		return ((const bitmapfile*) bitmap)->param1;
	}

	sint16 Bmp::GetParam2(id_bitmap_read bitmap)
	{
		return ((const bitmapfile*) bitmap)->param2;
	}

	sint32 Bmp::GetWidth(id_bitmap_read bitmap)
	{
		const bitmapinfo* Info = (const bitmapinfo*) (((bytes) bitmap) + sizeof(bitmapfile));
		return Info->width;
	}

	sint32 Bmp::GetHeight(id_bitmap_read bitmap)
	{
		const bitmapinfo* Info = (const bitmapinfo*) (((bytes) bitmap) + sizeof(bitmapfile));
		return Info->height;
	}

    sint16 Bmp::GetBitCount(id_bitmap_read bitmap)
    {
        const bitmapinfo* Info = (const bitmapinfo*) (((bytes) bitmap) + sizeof(bitmapfile));
        return Info->bitcount;
    }

	uint32 Bmp::GetImageSize(id_bitmap_read bitmap)
	{
		const bitmapinfo* Info = (const bitmapinfo*) (((bytes) bitmap) + sizeof(bitmapfile));
        return Info->size_image;
	}

    bytes Bmp::GetBits(id_bitmap_read bitmap)
    {
        return ((bytes) bitmap) + sizeof(bitmapfile) + sizeof(bitmapinfo);
    }

	uint08* Bmp::GetBits(id_bitmap bitmap)
	{
		return ((uint08*) bitmap) + sizeof(bitmapfile) + sizeof(bitmapinfo);
	}
}
