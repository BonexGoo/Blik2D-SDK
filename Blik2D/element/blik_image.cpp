﻿#include <blik.hpp>
#include "blik_image.hpp"

#include <format/blik_bmp.hpp>
#include <platform/blik_platform.hpp>
#include <addon/blik_addon.hpp>

namespace BLIK
{
    Image::Image()
    {
        m_fileformat = Format::Null;
        m_bitmap = nullptr;
        m_image_cache_max = 8;
        ResetData();
    }

    Image::Image(const Image& rhs)
    {
        operator=(rhs);
    }

    Image::~Image()
    {
        ResetBitmap();
    }

    Image& Image::operator=(const Image& rhs)
    {
        ResetBitmap();
        m_fileformat = rhs.m_fileformat;
        m_filepath = rhs.m_filepath;
        Bmp::Remove(m_bitmap);
        m_bitmap = (rhs.m_bitmap)? Bmp::Clone(rhs.m_bitmap) : nullptr;
        m_image_cache_max = rhs.m_image_cache_max;
        m_valid_rect.l = rhs.m_valid_rect.l;
        m_valid_rect.t = rhs.m_valid_rect.t;
        m_valid_rect.r = rhs.m_valid_rect.r;
        m_valid_rect.b = rhs.m_valid_rect.b;
        m_child_ixzone = rhs.m_child_ixzone;
        m_child_iyzone = rhs.m_child_iyzone;
        m_patch_xzone = rhs.m_patch_xzone;
        m_patch_yzone = rhs.m_patch_yzone;
        m_patch_calced_sum_width = rhs.m_patch_calced_sum_width;
        m_patch_calced_sum_height = rhs.m_patch_calced_sum_height;
        m_patch_calced_src_x = rhs.m_patch_calced_src_x;
        m_patch_calced_src_y = rhs.m_patch_calced_src_y;
        return *this;
    }

    Image& Image::SetName(chars name)
    {
        m_filepath = name;
        if(!m_filepath.Right(4).CompareNoCase(".bmp"))
            m_fileformat = Format::Bmp;
        else if(!m_filepath.Right(4).CompareNoCase(".jpg"))
            m_fileformat = Format::Jpg;
        else
        {
            m_filepath += ".bmp";
            m_fileformat = Format::Bmp;
        }
        return *this;
    }

    bool Image::Save(chars pathname) const
    {
        String PathnameWithSlash = pathname;
        if(0 < PathnameWithSlash.Length() && PathnameWithSlash[-2] != '/' && PathnameWithSlash[-2] != '\\')
            PathnameWithSlash += '/';

        const String Filename = m_filepath.Left(m_filepath.Length() - 4);
        id_asset BmpAsset = Asset::OpenForWrite(String::Format("%s%s.bmp", (chars) PathnameWithSlash, (chars) Filename), true);
        id_asset JsonAsset = Asset::OpenForWrite(String::Format("%s%s.json", (chars) PathnameWithSlash, (chars) Filename), true);
        if(!BmpAsset || !JsonAsset)
        {
            Asset::Close(BmpAsset);
            Asset::Close(JsonAsset);
            return false;
        }

        // Bitmap
        const uint32 BmpSize = Bmp::GetFileSizeWithoutBM(m_bitmap);
        Asset::Write(BmpAsset, (bytes) "BM", 2);
        Asset::Write(BmpAsset, (bytes) m_bitmap, BmpSize);
        Asset::Close(BmpAsset);

        // Data
        Context Json;
        Json.At("valid_rect").At("l").Set(String::FromInteger(m_valid_rect.l));
        Json.At("valid_rect").At("t").Set(String::FromInteger(m_valid_rect.t));
        Json.At("valid_rect").At("r").Set(String::FromInteger(m_valid_rect.r));
        Json.At("valid_rect").At("b").Set(String::FromInteger(m_valid_rect.b));

        for(sint32 i = 0; i < m_child_ixzone.Count(); ++i)
        {
            Json.At("child_ixzone").At(i).At("i").Set(String::FromInteger(m_child_ixzone[i].i));
            Json.At("child_ixzone").At(i).At("iend").Set(String::FromInteger(m_child_ixzone[i].iend));
            Json.At("child_ixzone").At(i).At("x").Set(String::FromInteger(m_child_ixzone[i].x));
            Json.At("child_ixzone").At(i).At("xend").Set(String::FromInteger(m_child_ixzone[i].xend));
        }
        for(sint32 i = 0; i < m_child_iyzone.Count(); ++i)
        {
            Json.At("child_iyzone").At(i).At("i").Set(String::FromInteger(m_child_iyzone[i].i));
            Json.At("child_iyzone").At(i).At("iend").Set(String::FromInteger(m_child_iyzone[i].iend));
            Json.At("child_iyzone").At(i).At("y").Set(String::FromInteger(m_child_iyzone[i].y));
            Json.At("child_iyzone").At(i).At("yend").Set(String::FromInteger(m_child_iyzone[i].yend));
        }

        for(sint32 i = 0; i < m_patch_xzone.Count(); ++i)
        {
            Json.At("patch_xzone").At(i).At("x").Set(String::FromInteger(m_patch_xzone[i].x));
            Json.At("patch_xzone").At(i).At("xend").Set(String::FromInteger(m_patch_xzone[i].xend));
        }
        for(sint32 i = 0; i < m_patch_yzone.Count(); ++i)
        {
            Json.At("patch_yzone").At(i).At("y").Set(String::FromInteger(m_patch_yzone[i].y));
            Json.At("patch_yzone").At(i).At("yend").Set(String::FromInteger(m_patch_yzone[i].yend));
        }

        String JsonString = Json.SaveJson();
        Asset::Write(JsonAsset, (bytes)(chars) JsonString, JsonString.Length());
        Asset::Close(JsonAsset);
        return true;
    }

    bool Image::Load(id_assetpath_read assetpath)
    {
        // 초기화
        ResetBitmap();
        ResetData();

        // 파일오픈
        if(m_fileformat == Format::Null) return false;
        id_asset_read ImageAsset = Asset::OpenForRead(String::Format("%s", (chars) m_filepath), assetpath);
        if(!ImageAsset) return false;
        const sint32 ImageAssetFilesize = Asset::Size(ImageAsset);
        if(ImageAssetFilesize == 0)
        {
            Asset::Close(ImageAsset);
            return false;
        }
        const String JsonFilename = m_filepath.Left(m_filepath.Length() - 4);
        id_asset_read JsonAsset = Asset::OpenForRead(String::Format("%s.json", (chars) JsonFilename), assetpath);

        // 디코딩
        if(m_fileformat == Format::Bmp)
        {
            buffer BmpBuffer = Buffer::Alloc(BLIK_DBG ImageAssetFilesize - 2);
            Asset::Skip(ImageAsset, 2);
            Asset::Read(ImageAsset, (uint08*) BmpBuffer, ImageAssetFilesize - 2);
            Asset::Close(ImageAsset);
            m_bitmap = Bmp::Clone((id_bitmap) BmpBuffer);
            Buffer::Free(BmpBuffer);
        }
        else if(m_fileformat == Format::Jpg)
        {
            buffer JpgBuffer = Buffer::Alloc(BLIK_DBG ImageAssetFilesize);
            Asset::Read(ImageAsset, (uint08*) JpgBuffer, ImageAssetFilesize);
            Asset::Close(ImageAsset);
            m_bitmap = AddOn::Jpg::ToBmp((bytes) JpgBuffer, ImageAssetFilesize);
            Buffer::Free(JpgBuffer);
        }
        RestoreFromMagentaAlpha();
        const sint32 Width = Bmp::GetWidth(m_bitmap);
        const sint32 Height = Bmp::GetHeight(m_bitmap);

        // 데이터화
        if(JsonAsset)
        {
            const sint32 JsonSize = Asset::Size(JsonAsset);
            buffer JsonBuffer = Buffer::Alloc(BLIK_DBG JsonSize + 1);
            Asset::Read(JsonAsset, (uint08*) JsonBuffer, JsonSize);
            ((char*) JsonBuffer)[JsonSize] = '\0';
            Asset::Close(JsonAsset);

            Context Json;
            Json.LoadJson(JsonBuffer);

            BLIK_ASSERT("valid_rect이 없습니다", Json("valid_rect").IsValid());
            m_valid_rect.l = Json("valid_rect")("l").GetInt();
            m_valid_rect.t = Json("valid_rect")("t").GetInt();
            m_valid_rect.r = Json("valid_rect")("r").GetInt();
            m_valid_rect.b = Json("valid_rect")("b").GetInt();
            BLIK_ASSERT("m_valid_rect가 잘못되었습니다", m_valid_rect.l < m_valid_rect.r && m_valid_rect.t < m_valid_rect.b);

            for(sint32 i = 0, iend = Json("child_ixzone").LengthOfIndexable(); i < iend; ++i)
            {
                m_child_ixzone.AtAdding();
                m_child_ixzone.At(-1).i = Json("child_ixzone")[i]("i").GetInt();
                m_child_ixzone.At(-1).iend = Json("child_ixzone")[i]("iend").GetInt();
                m_child_ixzone.At(-1).x = Json("child_ixzone")[i]("x").GetInt();
                m_child_ixzone.At(-1).xend = Json("child_ixzone")[i]("xend").GetInt();
            }
            for(sint32 i = 0, iend = Json("child_iyzone").LengthOfIndexable(); i < iend; ++i)
            {
                m_child_iyzone.AtAdding();
                m_child_iyzone.At(-1).i = Json("child_iyzone")[i]("i").GetInt();
                m_child_iyzone.At(-1).iend = Json("child_iyzone")[i]("iend").GetInt();
                m_child_iyzone.At(-1).y = Json("child_iyzone")[i]("y").GetInt();
                m_child_iyzone.At(-1).yend = Json("child_iyzone")[i]("yend").GetInt();
            }

            BLIK_ASSERT("patch_xzone이 없습니다", Json("patch_xzone").IsValid());
            for(sint32 i = 0, iend = Json("patch_xzone").LengthOfIndexable(); i < iend; ++i)
            {
                m_patch_xzone.AtAdding();
                m_patch_xzone.At(-1).x = Json("patch_xzone")[i]("x").GetInt();
                BLIK_ASSERT("patch_xzone이 잘못되었습니다", m_patch_xzone.Count() < 2 || m_patch_xzone.At(-2).xend <= m_patch_xzone.At(-1).x);
                m_patch_xzone.At(-1).xend = Json("patch_xzone")[i]("xend").GetInt();
                BLIK_ASSERT("patch_xzone이 잘못되었습니다", m_patch_xzone.At(-1).x < m_patch_xzone.At(-1).xend);
            }
            BLIK_ASSERT("patch_yzone이 없습니다", Json("patch_yzone").IsValid());
            for(sint32 i = 0, iend = Json("patch_yzone").LengthOfIndexable(); i < iend; ++i)
            {
                m_patch_yzone.AtAdding();
                m_patch_yzone.At(-1).y = Json("patch_yzone")[i]("y").GetInt();
                BLIK_ASSERT("patch_yzone이 잘못되었습니다", m_patch_yzone.Count() < 2 || m_patch_yzone.At(-2).yend <= m_patch_yzone.At(-1).y);
                m_patch_yzone.At(-1).yend = Json("patch_yzone")[i]("yend").GetInt();
                BLIK_ASSERT("patch_yzone이 잘못되었습니다", m_patch_yzone.At(-1).y < m_patch_yzone.At(-1).yend);
            }
        }
        else MakeData(0, 0, Width, Height);
        RecalcData();
        return true;
    }

    bool Image::LoadBitmap(id_bitmap_read bitmap)
    {
        // 초기화
        ResetBitmap();
        ResetData();

        // 복제
        const sint32 Width = Bmp::GetWidth(bitmap);
        const sint32 Height = Bmp::GetHeight(bitmap);
        m_bitmap = Bmp::Clone(bitmap);

        // 데이터화
        MakeData(0, 0, Width, Height);
        RecalcData();
        return true;
    }

    bool Image::LoadBitmapFromBits(bytes bits, sint32 width, sint32 height, sint32 bitcount, bool flip)
    {
        // 초기화
        ResetBitmap();
        ResetData();

        // 복제
        m_bitmap = Bmp::CloneFromBits(bits, width, height, bitcount, flip);

        // 데이터화
        MakeData(0, 0, width, height);
        RecalcData();
        return true;
    }

    bool Image::LoadUIBitmap(id_bitmap_read bitmap)
    {
        // 초기화
        ResetBitmap();
        ResetData();

        // 최소화후 복사
        const sint32 Width = Bmp::GetWidth(bitmap);
        const sint32 Height = Bmp::GetHeight(bitmap);
        const Bmp::bitmappixel* Bits = (const Bmp::bitmappixel*) Bmp::GetBits(bitmap);
        rect128 RectTest = {1, 1, Width - 1, Height - 1};
        Bmp::MinTest(bitmap, RectTest);
        m_bitmap = Bmp::Copy(bitmap, RectTest.l, RectTest.t, RectTest.r, RectTest.b);
        const sint32 XAdd = -RectTest.l;
        const sint32 YAdd = -RectTest.t;
        const sint32 NewWidth = RectTest.r - RectTest.l;
        const sint32 NewHeight = RectTest.b - RectTest.t;

        // 데이터화
        rect128 valid_rect;
        valid_rect.l = 0x0FFFFFFF;
        valid_rect.t = 0x0FFFFFFF;
        valid_rect.r = -0x0FFFFFFF;
        valid_rect.b = -0x0FFFFFFF;

        // rect수집
        for(sint32 x = 1; x < Width - 1; ++x)
        {
            const bool IsRed = (Bits[x + 0 * Width].argb == 0xFFFF0000);
            const bool IsBlue = (Bits[x + 0 * Width].argb == 0xFF0000FF);
            if(IsRed || IsBlue)
            {
                valid_rect.l = Math::Min(valid_rect.l, x);
                valid_rect.r = Math::Max(valid_rect.r, x + 1);
            }
        }
        for(sint32 y = 1; y < Height - 1; ++y)
        {
            const bool IsRed = (Bits[(Width - 1) + (Height - 1 - y) * Width].argb == 0xFFFF0000);
            const bool IsBlue = (Bits[(Width - 1) + (Height - 1 - y) * Width].argb == 0xFF0000FF);
            if(IsRed || IsBlue)
            {
                valid_rect.t = Math::Min(valid_rect.t, y);
                valid_rect.b = Math::Max(valid_rect.b, y + 1);
            }
        }

        // rect정보화 및 예외처리
        if(valid_rect.l != 0x0FFFFFFF && valid_rect.t != 0x0FFFFFFF)
        {
            m_valid_rect.l = valid_rect.l + XAdd;
            m_valid_rect.t = valid_rect.t + YAdd;
            m_valid_rect.r = valid_rect.r + XAdd;
            m_valid_rect.b = valid_rect.b + YAdd;
        }
        else
        {
            m_valid_rect.l = 0;
            m_valid_rect.t = 0;
            m_valid_rect.r = NewWidth;
            m_valid_rect.b = NewHeight;
        }

        // patch수집
        for(sint32 x = 1, oldx = -1; x < Width; ++x)
        {
            const bool IsRed = (Bits[x + (Height - 1) * Width].argb == 0xFFFF0000);
            if(oldx == -1 && IsRed) oldx = x;
            else if(oldx != -1 && !IsRed)
            {
                m_patch_xzone.AtAdding();
                m_patch_xzone.At(-1).x = oldx + XAdd;
                m_patch_xzone.At(-1).xend = x + XAdd;
                oldx = -1;
            }
        }
        for(sint32 y = 1, oldy = -1; y < Height; ++y)
        {
            const bool IsRed = (Bits[0 + (Height - 1 - y) * Width].argb == 0xFFFF0000);
            if(oldy == -1 && IsRed) oldy = y;
            else if(oldy != -1 && !IsRed)
            {
                m_patch_yzone.AtAdding();
                m_patch_yzone.At(-1).y = oldy + YAdd;
                m_patch_yzone.At(-1).yend = y + YAdd;
                oldy = -1;
            }
        }

        // patch예외처리
        if(m_patch_xzone.Count() == 0)
        {
            m_patch_xzone.AtAdding();
            m_patch_xzone.At(-1).x = m_valid_rect.l;
            m_patch_xzone.At(-1).xend = m_valid_rect.r;
        }
        if(m_patch_yzone.Count() == 0)
        {
            m_patch_yzone.AtAdding();
            m_patch_yzone.At(-1).y = m_valid_rect.t;
            m_patch_yzone.At(-1).yend = m_valid_rect.b;
        }
        RecalcData(); // child정보의 가공을 위한 선행실행

        // child수집가공
        for(sint32 x = 1, oldx = -1; x < Width - 1; ++x)
        {
            const bool IsBlue = (Bits[x + 0 * Width].argb == 0xFF0000FF);
            if(oldx == -1 && IsBlue) oldx = x;
            else if(oldx != -1 && !IsBlue)
            {
                m_child_ixzone.AtAdding();
                const sint32 X = oldx + XAdd;
                if(X < m_patch_calced_src_x[0])
                {
                    m_child_ixzone.At(-1).i = -1;
                    m_child_ixzone.At(-1).x = X - m_patch_calced_src_x[0];
                }
                else if(m_patch_calced_src_x[-1] <= X)
                {
                    m_child_ixzone.At(-1).i = m_patch_calced_src_x.Count() - 1;
                    m_child_ixzone.At(-1).x = X - m_patch_calced_src_x[-1];
                }
                else for(sint32 i = 0, iend = m_patch_calced_src_x.Count() - 1; i < iend; ++i)
                {
                    if(X < m_patch_calced_src_x[i + 1])
                    {
                        m_child_ixzone.At(-1).i = i;
                        m_child_ixzone.At(-1).x = 0x1000 * (X - m_patch_calced_src_x[i]) / (m_patch_calced_src_x[i + 1] - m_patch_calced_src_x[i]);
                        break;
                    }
                }
                const sint32 XEnd = x + XAdd;
                if(XEnd < m_patch_calced_src_x[0])
                {
                    m_child_ixzone.At(-1).iend = -1;
                    m_child_ixzone.At(-1).xend = XEnd - m_patch_calced_src_x[0];
                }
                else if(m_patch_calced_src_x[-1] <= XEnd)
                {
                    m_child_ixzone.At(-1).iend = m_patch_calced_src_x.Count() - 1;
                    m_child_ixzone.At(-1).xend = XEnd - m_patch_calced_src_x[-1];
                }
                else for(sint32 i = 0, iend = m_patch_calced_src_x.Count() - 1; i < iend; ++i)
                {
                    if(XEnd < m_patch_calced_src_x[i + 1])
                    {
                        m_child_ixzone.At(-1).iend = i;
                        m_child_ixzone.At(-1).xend = 0x1000 * (XEnd - m_patch_calced_src_x[i]) / (m_patch_calced_src_x[i + 1] - m_patch_calced_src_x[i]);
                        break;
                    }
                }
                oldx = -1;
            }
        }
        for(sint32 y = 1, oldy = -1; y < Height - 1; ++y)
        {
            const bool IsBlue = (Bits[(Width - 1) + (Height - 1 - y) * Width].argb == 0xFF0000FF);
            if(oldy == -1 && IsBlue) oldy = y;
            else if(oldy != -1 && !IsBlue)
            {
                m_child_iyzone.AtAdding();
                const sint32 Y = oldy + YAdd;
                if(Y < m_patch_calced_src_y[0])
                {
                    m_child_iyzone.At(-1).i = -1;
                    m_child_iyzone.At(-1).y = Y - m_patch_calced_src_y[0];
                }
                else if(m_patch_calced_src_y[-1] <= Y)
                {
                    m_child_iyzone.At(-1).i = m_patch_calced_src_y.Count() - 1;
                    m_child_iyzone.At(-1).y = Y - m_patch_calced_src_y[-1];
                }
                else for(sint32 i = 0, iend = m_patch_calced_src_y.Count() - 1; i < iend; ++i)
                {
                    if(Y < m_patch_calced_src_y[i + 1])
                    {
                        m_child_iyzone.At(-1).i = i;
                        m_child_iyzone.At(-1).y = 0x1000 * (Y - m_patch_calced_src_y[i]) / (m_patch_calced_src_y[i + 1] - m_patch_calced_src_y[i]);
                        break;
                    }
                }
                const sint32 YEnd = y + YAdd;
                if(YEnd < m_patch_calced_src_y[0])
                {
                    m_child_iyzone.At(-1).iend = -1;
                    m_child_iyzone.At(-1).yend = YEnd - m_patch_calced_src_y[0];
                }
                else if(m_patch_calced_src_y[-1] <= YEnd)
                {
                    m_child_iyzone.At(-1).iend = m_patch_calced_src_y.Count() - 1;
                    m_child_iyzone.At(-1).yend = YEnd - m_patch_calced_src_y[-1];
                }
                else for(sint32 i = 0, iend = m_patch_calced_src_y.Count() - 1; i < iend; ++i)
                {
                    if(YEnd < m_patch_calced_src_y[i + 1])
                    {
                        m_child_iyzone.At(-1).iend = i;
                        m_child_iyzone.At(-1).yend = 0x1000 * (YEnd - m_patch_calced_src_y[i]) / (m_patch_calced_src_y[i + 1] - m_patch_calced_src_y[i]);
                        break;
                    }
                }
                oldy = -1;
            }
        }
        return true;
    }

    void Image::Crear()
    {
        ResetBitmap();
        ResetData();
    }

    id_bitmap Image::CopiedBitmap(sint32 l, sint32 t, sint32 r, sint32 b) const
    {
        if(r == -1) r = Bmp::GetWidth(m_bitmap);
        if(b == -1) b = Bmp::GetHeight(m_bitmap);
        return (m_bitmap)? Bmp::Copy(m_bitmap, l, t, r, b) : nullptr;
    }

    void Image::ChangeToMagentaAlpha()
    {
        if(!m_bitmap) return;
        Bmp::ChangeColor(m_bitmap, 0x00000000, 0x00FF00FF);
    }

    void Image::RestoreFromMagentaAlpha()
    {
        if(!m_bitmap) return;
        Bmp::ChangeColor(m_bitmap, 0x00FF00FF, 0x00000000);
    }

    void Image::ReplaceAlphaChannelBy(id_bitmap_read src)
    {
        BLIK_ASSERT("src가 없거나 32비트 비트맵이 아닙니다", src && Bmp::GetBitCount(src) == 32);
        if(!m_bitmap || Bmp::GetBitCount(m_bitmap) != 32) return;

        const sint32 DstWidth = Bmp::GetWidth(m_bitmap);
        const sint32 DstHeight = Bmp::GetHeight(m_bitmap);
        auto DstBits = (Bmp::bitmappixel*) Bmp::GetBits(m_bitmap);
        const sint32 SrcWidth = Bmp::GetWidth(src);
        const sint32 SrcHeight = Bmp::GetHeight(src);
        auto SrcBits = (const Bmp::bitmappixel*) Bmp::GetBits(src);

        if(DstWidth == SrcWidth && DstHeight == SrcHeight)
        {
            for(sint32 y = 0; y < DstHeight; ++y)
            for(sint32 x = 0; x < DstWidth; ++x)
            {
                sint32 CurIndex = x + y * DstWidth;
                DstBits[CurIndex].a = SrcBits[CurIndex].a;
            }
        }
        else
        {
            for(sint32 y = 0; y < DstHeight; ++y)
            for(sint32 x = 0; x < DstWidth; ++x)
                DstBits[x + y * DstWidth].a = SrcBits[(x * SrcWidth / DstWidth) + (y * SrcHeight / DstHeight) * SrcWidth].a;
        }
    }

    sint32 Image::GetImageWidth() const
    {
        return Bmp::GetWidth(m_bitmap);
    }

    sint32 Image::GetImageHeight() const
    {
        return Bmp::GetHeight(m_bitmap);
    }

    Rect Image::CalcChildRect(const Rect& guide, sint32 ix, sint32 iy, sint32 xcount, sint32 ycount) const
    {
        BLIK_ASSERT("xcount는 1이상의 값이어야 합니다", 1 <= xcount);
        BLIK_ASSERT("ycount는 1이상의 값이어야 합니다", 1 <= ycount);

        UpdatePatchBy(guide.Width(), guide.Height());
        const sint32 x1 = Math::Min(Math::Max(0, ix), m_child_ixzone.Count() - 1);
        const sint32 y1 = Math::Min(Math::Max(0, iy), m_child_iyzone.Count() - 1);
        const sint32 x2 = Math::Min(Math::Max(0, ix + xcount - 1), m_child_ixzone.Count() - 1);
        const sint32 y2 = Math::Min(Math::Max(0, iy + ycount - 1), m_child_iyzone.Count() - 1);

        const float L = [this](const sint32 x1)->float
        {
            const sint32 i = m_child_ixzone[x1].i, x = m_child_ixzone[x1].x;
            if(i < 0) return m_patch_cached_dst_x[0] + x;
            else if(m_patch_cached_dst_x.Count() - 1 <= i) return m_patch_cached_dst_x[-1] + x;
            return m_patch_cached_dst_x[i] + (m_patch_cached_dst_x[i + 1] - m_patch_cached_dst_x[i]) * x / 0x1000;
        }(x1);
        const float T = [this](const sint32 y1)->float
        {
            const sint32 i = m_child_iyzone[y1].i, y = m_child_iyzone[y1].y;
            if(i < 0) return m_patch_cached_dst_y[0] + y;
            else if(m_patch_cached_dst_y.Count() - 1 <= i) return m_patch_cached_dst_y[-1] + y;
            return m_patch_cached_dst_y[i] + (m_patch_cached_dst_y[i + 1] - m_patch_cached_dst_y[i]) * y / 0x1000;
        }(y1);
        const float R = [this](const sint32 x2)->float
        {
            const sint32 i = m_child_ixzone[x2].iend, x = m_child_ixzone[x2].xend;
            if(i < 0) return m_patch_cached_dst_x[0] + x;
            else if(m_patch_cached_dst_x.Count() - 1 <= i) return m_patch_cached_dst_x[-1] + x;
            return m_patch_cached_dst_x[i] + (m_patch_cached_dst_x[i + 1] - m_patch_cached_dst_x[i]) * x / 0x1000;
        }(x2);
        const float B = [this](const sint32 y2)->float
        {
            const sint32 i = m_child_iyzone[y2].iend, y = m_child_iyzone[y2].yend;
            if(i < 0) return m_patch_cached_dst_y[0] + y;
            else if(m_patch_cached_dst_y.Count() - 1 <= i) return m_patch_cached_dst_y[-1] + y;
            return m_patch_cached_dst_y[i] + (m_patch_cached_dst_y[i + 1] - m_patch_cached_dst_y[i]) * y / 0x1000;
        }(y2);
        return Rect(guide.l + L, guide.t + T, guide.l + R, guide.t + B);
    }

    bool Image::UpdatePatchBy(float w, float h) const
    {
        BLIK_ASSERT("m_patch_cached_dst_x이 할당되어 있지 않습니다", 0 < m_patch_cached_dst_x.Count());
        BLIK_ASSERT("m_patch_cached_dst_y이 할당되어 있지 않습니다", 0 < m_patch_cached_dst_y.Count());

        if(m_patch_cached_dst_terms_w != w)
        {
            m_patch_cached_dst_terms_w = w;
            m_patch_cached_dst_visible_w = false;
            if(sint32* DstX = &m_patch_cached_dst_x.At(0))
            {
                const sint32 SrcBegin = Math::Min(0, PatchL());
                const sint32 SrcEnd = Math::Max(PatchR(), GetImageWidth());
                const sint32 SrcFixationSize = SrcEnd - SrcBegin - PatchSumWidth();
                const sint32 DstBegin = SrcBegin - L();
                const float DstEnd = w + SrcEnd - R();
                const float DstSize = DstEnd - DstBegin;
                if(0 < DstSize)
                {
                    m_patch_cached_dst_visible_w = true;
                    const bool PatchVisibled = (SrcFixationSize < DstSize);
                    const float PatchRate = (PatchVisibled)? (DstSize - SrcFixationSize) / PatchSumWidth() : 0;
                    const float FixationRate = (PatchVisibled)? 1 : DstSize / SrcFixationSize;
                    *(DstX++) = (sint32) DstBegin;
                    float DstPos = DstBegin;
                    if(0 < PatchL())
                        *(DstX++) = (sint32) (DstPos += PatchX(0) * FixationRate);
                    for(sint32 i = 0, iend = PatchXCount() - 1; i < iend; ++i)
                    {
                        *(DstX++) = (sint32) (DstPos += (PatchXEnd(i) - PatchX(i)) * PatchRate);
                        *(DstX++) = (sint32) (DstPos += (PatchX(i + 1) - PatchXEnd(i)) * FixationRate);
                    }
                    if(PatchR() < GetImageWidth())
                        *(DstX++) = (sint32) (DstPos += (PatchXEnd(-1) - PatchX(-1)) * PatchRate);
                    *(DstX++) = (sint32) DstEnd;
                }
            }
        }
        if(m_patch_cached_dst_terms_h != h)
        {
            m_patch_cached_dst_terms_h = h;
            m_patch_cached_dst_visible_h = false;
            if(sint32* DstY = &m_patch_cached_dst_y.At(0))
            {
                const sint32 SrcBegin = Math::Min(0, PatchT());
                const sint32 SrcEnd = Math::Max(PatchB(), GetImageHeight());
                const sint32 SrcFixationSize = SrcEnd - SrcBegin - PatchSumHeight();
                const sint32 DstBegin = SrcBegin - T();
                const float DstEnd = h + SrcEnd - B();
                const float DstSize = DstEnd - DstBegin;
                if(0 < DstSize)
                {
                    m_patch_cached_dst_visible_h = true;
                    const bool PatchVisibled = (SrcFixationSize < DstSize);
                    const float PatchRate = (PatchVisibled)? (DstSize - SrcFixationSize) / PatchSumHeight() : 0;
                    const float FixationRate = (PatchVisibled)? 1 : DstSize / SrcFixationSize;
                    *(DstY++) = (sint32) DstBegin;
                    float DstPos = (sint32) DstBegin;
                    if(0 < PatchT())
                        *(DstY++) = (sint32) (DstPos += PatchY(0) * FixationRate);
                    for(sint32 i = 0, iend = PatchYCount() - 1; i < iend; ++i)
                    {
                        *(DstY++) = (sint32) (DstPos += (PatchYEnd(i) - PatchY(i)) * PatchRate);
                        *(DstY++) = (sint32) (DstPos += (PatchY(i + 1) - PatchYEnd(i)) * FixationRate);
                    }
                    if(PatchB() < GetImageHeight())
                        *(DstY++) = (sint32) (DstPos += (PatchYEnd(-1) - PatchY(-1)) * PatchRate);
                    *(DstY++) = (sint32) DstEnd;
                }
            }
        }
        return (m_patch_cached_dst_visible_w && m_patch_cached_dst_visible_h);
    }

    void Image::ResetBitmap()
    {
        Bmp::Remove(m_bitmap);
        m_bitmap = nullptr;
        for(sint32 i = 0, iend = m_image_cached_map.Count(); i < iend; ++i)
        {
            ImageMap& OldImageMap = *m_image_cached_map.AccessByOrder(i);
            for(sint32 j = 0, jend = OldImageMap.Count(); j < jend; ++j)
                Platform::Graphics::RemoveImage(*OldImageMap.AccessByOrder(j));
        }
        m_image_cached_map.Reset();
        while(m_image_cached_queue.Count())
            delete m_image_cached_queue.Dequeue();
    }

    void Image::ResetData()
    {
        m_valid_rect.l = 0;
        m_valid_rect.t = 0;
        m_valid_rect.r = 0;
        m_valid_rect.b = 0;
        m_child_ixzone.Clear();
        m_child_iyzone.Clear();
        m_patch_xzone.Clear();
        m_patch_yzone.Clear();
        m_patch_calced_sum_width = 0;
        m_patch_calced_sum_height = 0;
        m_patch_calced_src_x.Clear();
        m_patch_calced_src_y.Clear();
        m_patch_cached_dst_terms_w = -0xFFFF;
        m_patch_cached_dst_terms_h = -0xFFFF;
        m_patch_cached_dst_visible_w = false;
        m_patch_cached_dst_visible_h = false;
        m_patch_cached_dst_x.Clear();
        m_patch_cached_dst_y.Clear();
    }

    void Image::MakeData(sint32 l, sint32 t, sint32 r, sint32 b)
    {
        m_valid_rect.l = l;
        m_valid_rect.t = t;
        m_valid_rect.r = r;
        m_valid_rect.b = b;
        m_patch_xzone.AtAdding();
        m_patch_xzone.At(-1).x = l;
        m_patch_xzone.At(-1).xend = r;
        m_patch_yzone.AtAdding();
        m_patch_yzone.At(-1).y = t;
        m_patch_yzone.At(-1).yend = b;
    }

    void Image::RecalcData()
    {
        BLIK_ASSERT("m_patch_calced_sum_width이 초기화되어 있지 않습니다", m_patch_calced_sum_width == 0);
        BLIK_ASSERT("m_patch_calced_sum_height이 초기화되어 있지 않습니다", m_patch_calced_sum_height == 0);
        BLIK_ASSERT("m_patch_calced_src_x이 초기화되어 있지 않습니다", m_patch_calced_src_x.Count() == 0);
        BLIK_ASSERT("m_patch_calced_src_y이 초기화되어 있지 않습니다", m_patch_calced_src_y.Count() == 0);

        for(sint32 i = 0, iend = m_patch_xzone.Count(); i < iend; ++i)
            m_patch_calced_sum_width += m_patch_xzone[i].xend - m_patch_xzone[i].x;
        for(sint32 i = 0, iend = m_patch_yzone.Count(); i < iend; ++i)
            m_patch_calced_sum_height += m_patch_yzone[i].yend - m_patch_yzone[i].y;

        BLIK_ASSERT("m_patch_xzone이 없습니다", 0 < m_patch_xzone.Count());
        if(0 < PatchL())
            m_patch_calced_src_x.AtAdding() = 0;
        for(sint32 i = 0, iend = PatchXCount(); i < iend; ++i)
        {
            m_patch_calced_src_x.AtAdding() = PatchX(i);
            m_patch_calced_src_x.AtAdding() = PatchXEnd(i);
        }
        if(PatchR() < GetImageWidth())
            m_patch_calced_src_x.AtAdding() = GetImageWidth();
        BLIK_ASSERT("m_patch_cached_dst_x이 초기화되어 있지 않습니다", m_patch_cached_dst_x.Count() == 0);
        m_patch_cached_dst_x.AtWherever(m_patch_calced_src_x.Count() - 1);

        BLIK_ASSERT("m_patch_yzone이 없습니다", 0 < m_patch_yzone.Count());
        if(0 < PatchT())
            m_patch_calced_src_y.AtAdding() = 0;
        for(sint32 i = 0, iend = PatchYCount(); i < iend; ++i)
        {
            m_patch_calced_src_y.AtAdding() = PatchY(i);
            m_patch_calced_src_y.AtAdding() = PatchYEnd(i);
        }
        if(PatchB() < GetImageHeight())
            m_patch_calced_src_y.AtAdding() = GetImageHeight();
        BLIK_ASSERT("m_patch_cached_dst_x이 초기화되어 있지 않습니다", m_patch_cached_dst_y.Count() == 0);
        m_patch_cached_dst_y.AtWherever(m_patch_calced_src_y.Count() - 1);
    }

    id_image_read Image::GetImageCore(const Color& coloring, sint32 width, sint32 height) const
    {
        if(!m_bitmap) return nullptr;
        width = Math::Max(1, width);
        height = Math::Max(1, height);
        const uint64 SizingKey = ((uint64) width) | (((uint64) height) << 32);
        if(ImageMap* ResultMap = m_image_cached_map.Access(coloring.rgba))
        if(id_image* Result = ResultMap->Access(SizingKey))
            return *Result;

        // 최대수량을 초과한 오래된 캐시를 제거
        while(m_image_cache_max <= m_image_cached_queue.Count())
        {
            const CacheKeys* OldCacheKeys = m_image_cached_queue.Dequeue();
            ImageMap* OldImageMap = m_image_cached_map.Access(OldCacheKeys->coloring);
            BLIK_ASSERT("잘못된 시나리오입니다", OldImageMap);
            id_image* OldImage = OldImageMap->Access(OldCacheKeys->sizing);
            BLIK_ASSERT("잘못된 시나리오입니다", OldImage);
            Platform::Graphics::RemoveImage(*OldImage);
            OldImageMap->Remove(OldCacheKeys->sizing);
            if(OldImageMap->Count() == 0)
                m_image_cached_map.Remove(OldCacheKeys->coloring);
            delete OldCacheKeys;
        }
        CacheKeys* NewCacheKeys = new CacheKeys;
        NewCacheKeys->coloring = coloring.rgba;
        NewCacheKeys->sizing = SizingKey;
        m_image_cached_queue.Enqueue(NewCacheKeys);

        // 옵션대로 생성하여 캐시맵에 할당
        return (m_image_cached_map[coloring.rgba][SizingKey] =
            Platform::Graphics::CreateImage(m_bitmap, coloring, width, height));
    }
}
