#include <blik.hpp>
#include "blik_zay.hpp"

ZAY_DECLARE_VIEW("_defaultview_")
ZAY_VIEW_API OnCommand(CommandType, chars, id_share, id_cloned_share*) {}
ZAY_VIEW_API OnNotify(chars, chars, id_share, id_cloned_share*) {}
ZAY_VIEW_API OnGesture(GestureType, sint32, sint32) {}
ZAY_VIEW_API OnRender(ZayPanel& panel)
{ZAY_RGB(panel, 0x80, 0x80, 0x80) panel.fill();}

namespace BLIK
{
    class TouchRect
    {
    public:
        TouchRect() {BLIK_ASSERT("잘못된 시나리오입니다", false);}
        TouchRect(chars uiname, float l, float t, float r, float b, float zoom, ZayPanel::SubGestureCB cb, bool hoverpass)
        {
            mName = uiname;
            mL = l;
            mT = t;
            mR = r;
            mB = b;
            mZoom = zoom;
            mCB = cb;
            mHoverPass = hoverpass;
        }
        ~TouchRect() {}
        TouchRect(const TouchRect& rhs) {operator=(rhs);}
        TouchRect& operator=(const TouchRect& rhs)
        {BLIK_ASSERT("잘못된 시나리오입니다", false); return *this;}

    public:
        static buffer Create(chars uiname, float l, float t, float r, float b, float zoom, ZayPanel::SubGestureCB cb, bool hoverpass)
        {
            buffer NewBuffer = Buffer::AllocNoConstructorOnce<TouchRect>(BLIK_DBG 1);
            BLIK_CONSTRUCTOR(NewBuffer, 0, TouchRect, uiname, l, t, r, b, zoom, cb, hoverpass);
            return NewBuffer;
        }

    public:
        String mName;
        float mL;
        float mT;
        float mR;
        float mB;
        float mZoom;
        ZayPanel::SubGestureCB mCB;
        bool mHoverPass;
    };
    typedef Object<TouchRect> TouchRectObject;
    typedef Array<TouchRectObject> TouchRectObjects;

    class TouchCollector
    {
    public:
        TouchCollector()
        {
            mWidth = 0;
            mHeight = 0;
            mRefTouch = nullptr;
            mDirty = true;
        }
        ~TouchCollector() {}

    public:
        static TouchCollector* ST(chars name)
        {return &(*BLIK_STORAGE_SYS(Map<TouchCollector>))(name);}

    public:
        float mWidth;
        float mHeight;
        void* mRefTouch;
        bool mDirty;
        TouchRectObjects mTouchRects;
    };

    ZayObject::ZayObject()
    {
        m_resource = nullptr;
        m_frame_counter = 0;
        m_finder = nullptr;
        m_finder_data = nullptr;
        m_updater = nullptr;
        m_updater_data = nullptr;
        m_resizing_width = -1;
        m_resizing_height = -1;
    }

    ZayObject::~ZayObject()
    {
    }

    void ZayObject::bind(void* resource)
    {
        m_resource = resource;
    }

    void ZayObject::initialize()
    {
        BLIK_ASSERT("This function should not be called directly.", false);
    }

    void ZayObject::synchronize(SynchronizeModel model, sint32 param) const
    {
        BLIK_ASSERT("This function should not be called directly.", false);
    }

    void ZayObject::invalidate(sint32 count) const
    {
        BLIK_ASSERT("Updater가 없습니다", m_updater);
        if(m_updater && 0 < count)
            m_updater(m_updater_data, count);
    }

    void ZayObject::invalidate(chars uigroup) const
    {
        if(uigroup)
            TouchCollector::ST(uigroup)->mDirty = true;
        invalidate();
    }

    void ZayObject::invalidator(payload data, chars uigroup)
    {
        ((const ZayObject*) data)->invalidate(uigroup);
    }

    ZayObject* ZayObject::next(chars viewclass)
    {
        ZayObject* Result = nullptr;
        if(m_view.get())
            Result = (ZayObject*) Platform::SetNextViewClass(m_view, viewclass);
        return Result;
    }

    bool ZayObject::next(View* viewmanager)
    {
        bool Result = false;
        if(m_view.get())
            Result = Platform::SetNextViewManager(m_view, viewmanager);
        return Result;
    }

    void ZayObject::exit()
    {
        BLIK_ASSERT("Updater가 없습니다", m_updater);
        if(m_updater)
            m_updater(m_updater_data, -1);
    }

    bool ZayObject::valid(chars uiname) const
    {
        BLIK_ASSERT("Finder가 없습니다", m_finder);
        return !!m_finder(m_finder_data, uiname);
    }

    const rect128& ZayObject::rect(chars uiname) const
    {
        BLIK_ASSERT("Finder가 없습니다", m_finder);
        if(auto CurElement = (const ZayView::Element*) m_finder(m_finder_data, uiname))
            return CurElement->m_rect;
        static const rect128 NullRect = {0, 0, 0, 0};
        return NullRect;
    }

    const float ZayObject::zoom(chars uiname) const
    {
        BLIK_ASSERT("Finder가 없습니다", m_finder);
        if(auto CurElement = (const ZayView::Element*) m_finder(m_finder_data, uiname))
            return CurElement->m_zoom;
        return 1;
    }

    const point64& ZayObject::oldxy(chars uiname) const
    {
        BLIK_ASSERT("Finder가 없습니다", m_finder);
        if(auto CurElement = (const ZayView::Element*) m_finder(m_finder_data, uiname))
            return CurElement->m_saved_xy;
        static const point64 NullPoint = {0, 0};
        return NullPoint;
    }

    void ZayObject::resizeForced(sint32 w, sint32 h)
    {
        m_resizing_width = w;
        m_resizing_height = h;
    }

    bool ZayObject::getResizingValue(sint32& w, sint32& h)
    {
        bool Result = false;
        if(m_resizing_width != -1)
        {
            Result = true;
            w = m_resizing_width;
            m_resizing_width = -1;
        }
        if(m_resizing_height != -1)
        {
            Result = true;
            h = m_resizing_height;
            m_resizing_height = -1;
        }
        return Result;
    }

    ZayPanel::ZayPanel(float width, float height, const buffer touch)
        : m_width(width), m_height(height)
    {
        m_dirty = false;
        m_ref_surface = nullptr;
        m_ref_touch = touch;
        m_ref_touch_collector = nullptr;
        ((ZayView::Touch*) m_ref_touch)->ready((sint32) m_width, (sint32) m_height);

        m_stack_clip.AtAdding() = Clip(0, 0, m_width, m_height, true);
        m_stack_scissor.AtAdding() = Rect(0, 0, m_width, m_height);
        m_stack_color.AtAdding() = Color(Color::ColoringDefault);
        m_stack_font.AtAdding() = Font("Arial", 10);
        m_stack_zoom.AtAdding() = 1;

        m_clipped_width = m_width;
        m_clipped_height = m_height;
        m_child_image = nullptr;
        m_child_guide = Rect(0, 0, m_width, m_height);
        m_test_scissor = true;

        Platform::Graphics::SetScissor(0, 0, m_width, m_height);
        Platform::Graphics::SetColor(0xFF, 0xFF, 0xFF, 0xFF);
        Platform::Graphics::SetFont("Arial", 10);
        Platform::Graphics::SetZoom(1);
    }

    ZayPanel::ZayPanel(id_surface surface, float width, float height, chars uigroup)
        : m_width(width), m_height(height)
    {
        BLIK_ASSERT("surface가 nullptr입니다", surface);

        if(uigroup)
        {
            m_dirty = TouchCollector::ST(uigroup)->mDirty;
            TouchCollector::ST(uigroup)->mDirty = false;
        }
        else m_dirty = true;

        if(m_dirty)
        {
            m_ref_surface = surface;
            m_ref_touch = (uigroup)? TouchCollector::ST(uigroup)->mRefTouch : nullptr;
            m_ref_touch_collector = (uigroup)? TouchCollector::ST(uigroup) : nullptr;
            Platform::Graphics::BindSurface(m_ref_surface);
            if(m_ref_touch_collector)
            {
                ((TouchCollector*) m_ref_touch_collector)->mWidth = m_width;
                ((TouchCollector*) m_ref_touch_collector)->mHeight = m_height;
                ((TouchCollector*) m_ref_touch_collector)->mTouchRects.SubtractionAll();
            }

            m_stack_clip.AtAdding() = Clip(0, 0, m_width, m_height, true);
            m_stack_scissor.AtAdding() = Rect(0, 0, m_width, m_height);
            m_stack_color.AtAdding() = Color(Color::ColoringDefault);
            m_stack_font.AtAdding() = Font("Arial", 10);
            m_stack_zoom.AtAdding() = 1;

            m_clipped_width = m_width;
            m_clipped_height = m_height;
            m_child_image = nullptr;
            m_child_guide = Rect(0, 0, m_width, m_height);
            m_test_scissor = true;

            Platform::Graphics::SetScissor(0, 0, m_width, m_height);
            Platform::Graphics::SetColor(0xFF, 0xFF, 0xFF, 0xFF);
            Platform::Graphics::SetFont("Arial", 10);
            Platform::Graphics::SetZoom(1);
        }
        else m_ref_surface = nullptr;
    }

    ZayPanel::~ZayPanel()
    {
        Platform::Graphics::UnbindSurface(m_ref_surface);
    }

    void ZayPanel::erase() const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::EraseRect(LastClip.l, LastClip.t, LastClip.Width(), LastClip.Height());
    }

    void ZayPanel::fill() const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::FillRect(LastClip.l, LastClip.t, LastClip.Width(), LastClip.Height());
    }

    void ZayPanel::rect(float thick) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::DrawRect(LastClip.l, LastClip.t, LastClip.Width(), LastClip.Height(), thick);
    }

    void ZayPanel::line(const Point& begin, const Point& end, float thick) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        const Point PointAdd = Point(LastClip.l, LastClip.t);
        Platform::Graphics::DrawLine(begin + PointAdd, end + PointAdd, thick);
    }

    void ZayPanel::circle() const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::DrawCircle(LastClip.l, LastClip.t, LastClip.Width(), LastClip.Height());
    }

    void ZayPanel::bezier(const Vector& begin, const Vector& end, float thick) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        const Point PointAdd = Point(LastClip.l, LastClip.t);
        Platform::Graphics::DrawBezier(begin + PointAdd, end + PointAdd, thick);
    }

    void ZayPanel::polygon(Points p) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::FillPolygon(LastClip.l, LastClip.t, p);
    }

    void ZayPanel::polyline(Points p, float thick) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::DrawPolyLine(LastClip.l, LastClip.t, p, thick);
    }

    void ZayPanel::polybezier(Points p, float thick, bool showfirst, bool showlast) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        Platform::Graphics::DrawPolyBezier(LastClip.l, LastClip.t, p, thick, showfirst, showlast);
    }

    static inline sint32 GetXAlignCode(UIAlign align)
    {
        sint32 XAlignCode = 0;
        switch(align)
        {
        case UIA_LeftTop:    XAlignCode = 0; break; case UIA_CenterTop:    XAlignCode = 1; break; case UIA_RightTop:    XAlignCode = 2; break;
        case UIA_LeftMiddle: XAlignCode = 0; break; case UIA_CenterMiddle: XAlignCode = 1; break; case UIA_RightMiddle: XAlignCode = 2; break;
        case UIA_LeftBottom: XAlignCode = 0; break; case UIA_CenterBottom: XAlignCode = 1; break; case UIA_RightBottom: XAlignCode = 2; break;
        }
        return XAlignCode;
    }

    static inline sint32 GetYAlignCode(UIAlign align)
    {
        sint32 YAlignCode = 0;
        switch(align)
        {
        case UIA_LeftTop:    YAlignCode = 0; break; case UIA_CenterTop:    YAlignCode = 0; break; case UIA_RightTop:    YAlignCode = 0; break;
        case UIA_LeftMiddle: YAlignCode = 1; break; case UIA_CenterMiddle: YAlignCode = 1; break; case UIA_RightMiddle: YAlignCode = 1; break;
        case UIA_LeftBottom: YAlignCode = 2; break; case UIA_CenterBottom: YAlignCode = 2; break; case UIA_RightBottom: YAlignCode = 2; break;
        }
        return YAlignCode;
    }

    haschild ZayPanel::icon(const Image& image, UIAlign align, bool visible)
    {
        const Clip& LastClip = m_stack_clip[-1];
        const sint32 XAlignCode = GetXAlignCode(align);
        const sint32 YAlignCode = GetYAlignCode(align);
        const float DstX = ((XAlignCode == 0)? 0 : ((XAlignCode == 1)? (LastClip.Width() - image.GetWidth()) * 0.5f : LastClip.Width() - image.GetWidth()));
        const float DstY = ((YAlignCode == 0)? 0 : ((YAlignCode == 1)? (LastClip.Height() - image.GetHeight()) * 0.5f : LastClip.Height() - image.GetHeight()));

        if(visible)
        {
            const Color& LastColor = m_stack_color[-1];
            Platform::Graphics::DrawImage(image.GetImage(LastColor),
                0, 0, image.GetImageWidth(), image.GetImageHeight(),
                LastClip.l + DstX - image.L(), LastClip.t + DstY - image.T(),
                image.GetImageWidth(), image.GetImageHeight());
        }

        if(image.HasChild())
        {
            m_child_image = &image;
            m_child_guide = Rect(Point(LastClip.l + DstX, LastClip.t + DstY), Size(image.GetWidth(), image.GetHeight()));
            return haschild_ok;
        }
        return haschild_null;
    }

    haschild ZayPanel::icon(float x, float y, const Image& image, UIAlign align, bool visible)
    {
        const Clip& LastClip = m_stack_clip[-1];
        const sint32 XAlignCode = GetXAlignCode(align);
        const sint32 YAlignCode = GetYAlignCode(align);
        const float DstX = ((XAlignCode == 0)? x : ((XAlignCode == 1)? x - image.GetWidth() * 0.5f : x - image.GetWidth()));
        const float DstY = ((YAlignCode == 0)? y : ((YAlignCode == 1)? y - image.GetHeight() * 0.5f : y - image.GetHeight()));

        if(visible)
        {
            const Color& LastColor = m_stack_color[-1];
            Platform::Graphics::DrawImage(image.GetImage(LastColor),
                0, 0, image.GetImageWidth(), image.GetImageHeight(),
                LastClip.l + DstX - image.L(), LastClip.t + DstY - image.T(),
                image.GetImageWidth(), image.GetImageHeight());
        }

        if(image.HasChild())
        {
            m_child_image = &image;
            m_child_guide = Rect(Point(LastClip.l + DstX, LastClip.t + DstY), Size(image.GetWidth(), image.GetHeight()));
            return haschild_ok;
        }
        return haschild_null;
    }

    haschild ZayPanel::iconNative(id_image_read image, UIAlign align)
    {
        const Clip& LastClip = m_stack_clip[-1];
        const sint32 XAlignCode = GetXAlignCode(align);
        const sint32 YAlignCode = GetYAlignCode(align);
        const sint32 ImageWidth = Platform::Graphics::GetImageWidth(image);
        const sint32 ImageHeight = Platform::Graphics::GetImageHeight(image);
        const float DstX = ((XAlignCode == 0)? 0 : ((XAlignCode == 1)? (LastClip.Width() - ImageWidth) * 0.5f : LastClip.Width() - ImageWidth));
        const float DstY = ((YAlignCode == 0)? 0 : ((YAlignCode == 1)? (LastClip.Height() - ImageHeight) * 0.5f : LastClip.Height() - ImageHeight));

        Platform::Graphics::DrawImage(image, 0, 0, ImageWidth, ImageHeight,
            LastClip.l + DstX, LastClip.t + DstY, ImageWidth, ImageHeight);
        return haschild_null;
    }

    haschild ZayPanel::iconNative(float x, float y, id_image_read image, UIAlign align)
    {
        const Clip& LastClip = m_stack_clip[-1];
        const sint32 XAlignCode = GetXAlignCode(align);
        const sint32 YAlignCode = GetYAlignCode(align);
        const sint32 ImageWidth = Platform::Graphics::GetImageWidth(image);
        const sint32 ImageHeight = Platform::Graphics::GetImageHeight(image);
        const float DstX = ((XAlignCode == 0)? x : ((XAlignCode == 1)? x - ImageWidth * 0.5f : x - ImageWidth));
        const float DstY = ((YAlignCode == 0)? y : ((YAlignCode == 1)? y - ImageHeight * 0.5f : y - ImageHeight));

        Platform::Graphics::DrawImage(image, 0, 0, ImageWidth, ImageHeight,
            LastClip.l + DstX, LastClip.t + DstY, ImageWidth, ImageHeight);
        return haschild_null;
    }

    haschild ZayPanel::stretch(const Image& image, bool rebuild, bool visible)
    {
        const Clip& LastClip = m_stack_clip[-1];
        if(visible)
        {
            const float XRate = LastClip.Width() / image.GetWidth();
            const float YRate = LastClip.Height() / image.GetHeight();
            const float DstX = -image.L() * XRate;
            const float DstY = -image.T() * YRate;
            const sint32 DstWidth = (sint32) (image.GetImageWidth() * XRate);
            const sint32 DstHeight = (sint32) (image.GetImageHeight() * YRate);
            const sint32 ImageWidth = (rebuild)? DstWidth : image.GetImageWidth();
            const sint32 ImageHeight = (rebuild)? DstHeight : image.GetImageHeight();

            const Color& LastColor = m_stack_color[-1];
            Platform::Graphics::DrawImage(image.GetImage(LastColor, ImageWidth, ImageHeight),
                0, 0, ImageWidth, ImageHeight, LastClip.l + DstX, LastClip.t + DstY, DstWidth, DstHeight);
        }

        if(image.HasChild())
        {
            m_child_image = &image;
            m_child_guide = Rect(LastClip.l, LastClip.t, LastClip.r, LastClip.b);
            return haschild_ok;
        }
        return haschild_null;
    }

    haschild ZayPanel::stretchNative(id_image_read image) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        const sint32 ImageWidth = Platform::Graphics::GetImageWidth(image);
        const sint32 ImageHeight = Platform::Graphics::GetImageHeight(image);
        const float XRate = LastClip.Width() / ImageWidth;
        const float YRate = LastClip.Height() / ImageHeight;
        const sint32 DstWidth = (sint32) (ImageWidth * XRate);
        const sint32 DstHeight = (sint32) (ImageHeight * YRate);

        Platform::Graphics::DrawImage(image, 0, 0, ImageWidth, ImageHeight,
            LastClip.l, LastClip.t, DstWidth, DstHeight);
        return haschild_null;
    }

    haschild ZayPanel::ninepatch(const Image& image, bool visible)
    {
        const Clip& LastClip = m_stack_clip[-1];
        if(visible)
        if(image.UpdatePatchBy(LastClip.Width(), LastClip.Height()))
        {
            const sint32* PatchSrcX = image.PatchSrcXArray();
            const sint32* PatchSrcY = image.PatchSrcYArray();
            const sint32* PatchDstX = image.PatchDstXArray();
            const sint32* PatchDstY = image.PatchDstYArray();

            const Color& LastColor = m_stack_color[-1];
            for(sint32 y = 0, yend = image.PatchSrcYCount() - 1; y < yend; ++y)
            for(sint32 x = 0, xend = image.PatchSrcXCount() - 1; x < xend; ++x)
                Platform::Graphics::DrawImage(image.GetImage(LastColor),
                    PatchSrcX[x], PatchSrcY[y], PatchSrcX[x + 1] - PatchSrcX[x], PatchSrcY[y + 1] - PatchSrcY[y],
                    LastClip.l + PatchDstX[x], LastClip.t + PatchDstY[y], PatchDstX[x + 1] - PatchDstX[x], PatchDstY[y + 1] - PatchDstY[y]);
        }

        if(image.HasChild())
        {
            m_child_image = &image;
            m_child_guide = Rect(LastClip.l, LastClip.t, LastClip.r, LastClip.b);
            return haschild_ok;
        }
        return haschild_null;
    }

    void ZayPanel::pattern(const Image& image, UIAlign align, bool reversed_xorder, bool reversed_yorder) const
    {
        sint32 XAlignCode = 0;
        switch(align)
        {
        case UIA_LeftTop:    XAlignCode = 0; break; case UIA_CenterTop:    XAlignCode = 1; break; case UIA_RightTop:    XAlignCode = 2; break;
        case UIA_LeftMiddle: XAlignCode = 0; break; case UIA_CenterMiddle: XAlignCode = 1; break; case UIA_RightMiddle: XAlignCode = 2; break;
        case UIA_LeftBottom: XAlignCode = 0; break; case UIA_CenterBottom: XAlignCode = 1; break; case UIA_RightBottom: XAlignCode = 2; break;
        }
        sint32 YAlignCode = 0;
        switch(align)
        {
        case UIA_LeftTop:    YAlignCode = 0; break; case UIA_CenterTop:    YAlignCode = 0; break; case UIA_RightTop:    YAlignCode = 0; break;
        case UIA_LeftMiddle: YAlignCode = 1; break; case UIA_CenterMiddle: YAlignCode = 1; break; case UIA_RightMiddle: YAlignCode = 1; break;
        case UIA_LeftBottom: YAlignCode = 2; break; case UIA_CenterBottom: YAlignCode = 2; break; case UIA_RightBottom: YAlignCode = 2; break;
        }

        const Clip& LastClip = m_stack_clip[-1];
        const float DstXAdd = (XAlignCode == 0)? 0 : ((XAlignCode == 1)?
            Math::Mod((LastClip.Width() + image.GetWidth()) * 0.5f, image.GetWidth()) - image.GetWidth() :
            Math::Mod(LastClip.Width(), image.GetWidth()) - image.GetWidth());
        const float DstYAdd = (YAlignCode == 0)? 0 : ((YAlignCode == 1)?
            Math::Mod((LastClip.Height() + image.GetHeight()) * 0.5f, image.GetHeight()) - image.GetHeight() :
            Math::Mod(LastClip.Height(), image.GetHeight()) - image.GetHeight());
        const sint32 XCount = (sint32) ((LastClip.Width() + image.GetWidth() - DstXAdd) / image.GetWidth());
        const sint32 YCount = (sint32) ((LastClip.Height() + image.GetHeight() - DstYAdd) / image.GetHeight());
        const float XBegin = DstXAdd + ((reversed_xorder)? image.GetWidth() * (XCount - 1) : 0) - image.L();
        const float YBegin = DstYAdd + ((reversed_yorder)? image.GetHeight() * (YCount - 1) : 0) - image.T();
        const sint32 XStep = (reversed_xorder)? -image.GetWidth() : image.GetWidth();
        const sint32 YStep = (reversed_yorder)? -image.GetHeight() : image.GetHeight();

        const Color& LastColor = m_stack_color[-1];
        float DstY = YBegin;
        for(sint32 y = 0; y < YCount; ++y)
        {
            float DstX = XBegin;
            for(sint32 x = 0; x < XCount; ++x)
            {
                Platform::Graphics::DrawImage(image.GetImage(LastColor),
                    0, 0, image.GetImageWidth(), image.GetImageHeight(),
                    LastClip.l + DstX, LastClip.t + DstY,
                    image.GetImageWidth(), image.GetImageHeight());
                DstX += XStep;
            }
            DstY += YStep;
        }
    }

    bool ZayPanel::text(chars string, UIFontAlign align, UIFontElide elide) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        return Platform::Graphics::DrawString(LastClip.l, LastClip.t, LastClip.Width(), LastClip.Height(), string, align, elide);
    }

    void ZayPanel::text(float x, float y, chars string, UIFontAlign align) const
    {
        x += m_stack_clip[-1].l;
        y += m_stack_clip[-1].t;

        bool NeedWidth = false;
        switch(align)
        {
        case UIFA_LeftTop: break; case UIFA_CenterTop: case UIFA_RightTop: case UIFA_JustifyTop: NeedWidth = true; break;
        case UIFA_LeftMiddle: break; case UIFA_CenterMiddle: case UIFA_RightMiddle: case UIFA_JustifyMiddle: NeedWidth = true; break;
        case UIFA_LeftAscent: break; case UIFA_CenterAscent: case UIFA_RightAscent: case UIFA_JustifyAscent: NeedWidth = true; break;
        case UIFA_LeftBottom: break; case UIFA_CenterBottom: case UIFA_RightBottom: case UIFA_JustifyBottom: NeedWidth = true; break;
        }
        bool NeedHeight = false, NeedAscent = false;
        switch(align)
        {
        case UIFA_LeftTop: case UIFA_CenterTop: case UIFA_RightTop: case UIFA_JustifyTop: break;
        case UIFA_LeftMiddle: case UIFA_CenterMiddle: case UIFA_RightMiddle: case UIFA_JustifyMiddle: NeedHeight = true; break;
        case UIFA_LeftAscent: case UIFA_CenterAscent: case UIFA_RightAscent: case UIFA_JustifyAscent: NeedAscent = true; break;
        case UIFA_LeftBottom: case UIFA_CenterBottom: case UIFA_RightBottom: case UIFA_JustifyBottom: NeedHeight = true; break;
        }
        const sint32 Width = (NeedWidth)? Platform::Graphics::GetStringWidth(string) : 0;
        const sint32 Height = (NeedHeight)? Platform::Graphics::GetStringHeight() : 0;
        const sint32 Ascent = (NeedAscent)? Platform::Graphics::GetStringAscent() : 0;

        Rect CalcRect;
        switch(align)
        {
        case UIFA_LeftTop: CalcRect.l = x; CalcRect.t = y; break;
        case UIFA_CenterTop: CalcRect.l = x - Width * 0.5f; CalcRect.t = y; break;
        case UIFA_RightTop: CalcRect.l = x - Width; CalcRect.t = y; break;
        case UIFA_JustifyTop: CalcRect.l = x - Width * 0.5f; CalcRect.t = y; break;
        case UIFA_LeftMiddle: CalcRect.l = x; CalcRect.t = y - Height * 0.5f; break;
        case UIFA_CenterMiddle: CalcRect.l = x - Width * 0.5f; CalcRect.t = y - Height * 0.5f; break;
        case UIFA_RightMiddle: CalcRect.l = x - Width; CalcRect.t = y - Height * 0.5f; break;
        case UIFA_JustifyMiddle: CalcRect.l = x - Width * 0.5f; CalcRect.t = y - Height * 0.5f; break;
        case UIFA_LeftAscent: CalcRect.l = x; CalcRect.t = y - Ascent; break;
        case UIFA_CenterAscent: CalcRect.l = x - Width * 0.5f; CalcRect.t = y - Ascent; break;
        case UIFA_RightAscent: CalcRect.l = x - Width; CalcRect.t = y - Ascent; break;
        case UIFA_JustifyAscent: CalcRect.l = x - Width * 0.5f; CalcRect.t = y - Ascent; break;
        case UIFA_LeftBottom: CalcRect.l = x; CalcRect.t = y - Height; break;
        case UIFA_CenterBottom: CalcRect.l = x - Width * 0.5f; CalcRect.t = y - Height; break;
        case UIFA_RightBottom: CalcRect.l = x - Width; CalcRect.t = y - Height; break;
        case UIFA_JustifyBottom: CalcRect.l = x - Width * 0.5f; CalcRect.t = y - Height; break;
        }
        CalcRect.r = CalcRect.l + 0x2000; // 0xFFFF였으나 cocos2dx의 한계로 인하여 적당히 수정
        CalcRect.b = CalcRect.t + 0x2000;
        Platform::Graphics::DrawString(CalcRect.l, CalcRect.t, CalcRect.Width(), CalcRect.Height(), string, UIFA_LeftTop);
    }

    void ZayPanel::sub(chars uigroup, id_surface surface) const
    {
        auto CurCollector = TouchCollector::ST(uigroup);
        bool DirtyTest = CurCollector->mDirty;
        if(auto CurTouch = (ZayView::Touch*) m_ref_touch)
        {
            const Clip& LastClip = m_stack_clip[-1];
            const float& LastZoom = m_stack_zoom[-1];
            const float HRate = LastClip.Width() / CurCollector->mWidth;
            const float VRate = LastClip.Height() / CurCollector->mHeight;
            for(sint32 i = 0, iend = CurCollector->mTouchRects.Count(); i < iend; ++i)
            {
                const TouchRect& CurTouchRect = CurCollector->mTouchRects[i].ConstValue();
                CurTouch->update(CurTouchRect.mName,
                    LastClip.l + CurTouchRect.mL * CurTouchRect.mZoom * HRate,
                    LastClip.t + CurTouchRect.mT * CurTouchRect.mZoom * VRate,
                    LastClip.l + CurTouchRect.mR * CurTouchRect.mZoom * HRate,
                    LastClip.t + CurTouchRect.mB * CurTouchRect.mZoom * VRate,
                    LastZoom, CurTouchRect.mCB, CurTouchRect.mHoverPass, &DirtyTest);
            }
        }
        CurCollector->mRefTouch = m_ref_touch;
        CurCollector->mDirty = DirtyTest;

        stretchNative(Platform::Graphics::GetImageFromSurface(surface));
    }

    PanelState ZayPanel::state(chars uiname) const
    {
        if(auto CurTouch = (const ZayView::Touch*) m_ref_touch)
        if(auto CurElement = CurTouch->get(uiname, 1))
            return CurElement->GetState(m_ref_touch);
        return PS_Null;
    }

    Point ZayPanel::toview(float x, float y) const
    {
        const Clip& LastClip = m_stack_clip[-1];
        return Point(LastClip.l + x, LastClip.t + y);
    }

    void ZayPanel::test(UITestOrder order)
    {
        switch(order)
        {
        case UITO_ScissorOn: m_test_scissor = true; break;
        case UITO_ScissorOff: m_test_scissor = false; break;
        }
    }

    ZayPanel::StackBinder ZayPanel::_push_clip(float l, float t, float r, float b, bool doScissor)
    {
        Clip& NewClip = m_stack_clip.AtAdding();
        const Clip& LastClip = m_stack_clip[-2];
        NewClip = Clip(LastClip.l + l, LastClip.t + t, LastClip.l + r, LastClip.t + b, doScissor);

        if(doScissor && !_push_scissor(NewClip.l, NewClip.t, NewClip.r, NewClip.b))
        {
            m_stack_clip.SubtractionOne();
            return StackBinder(this);
        }

        m_clipped_width = NewClip.Width();
        m_clipped_height = NewClip.Height();
        return StackBinder(this, ST_Clip);
    }

    ZayPanel::StackBinder ZayPanel::_push_clip_ui(float l, float t, float r, float b, bool doScissor, chars uiname, SubGestureCB cb, bool hoverpass)
    {
        if(auto& CurBinder = _push_clip(l, t, r, b, doScissor))
        {
            if(auto CurCollector = (TouchCollector*) m_ref_touch_collector)
            {
                const Clip& LastClip = m_stack_clip[-1];
                const float& LastZoom = m_stack_zoom[-1];
                CurCollector->mTouchRects.AtAdding() =
                    TouchRect::Create(uiname, LastClip.l, LastClip.t, LastClip.r, LastClip.b, LastZoom, cb, hoverpass);
            }
            else if(auto CurTouch = (ZayView::Touch*) m_ref_touch)
            {
                const Clip& LastClip = m_stack_clip[-1];
                const float& LastZoom = m_stack_zoom[-1];
                CurTouch->update(uiname, LastClip.l, LastClip.t, LastClip.r, LastClip.b, LastZoom, cb, hoverpass);
            }
            return CurBinder;
        }
        return StackBinder(this);
    }

    ZayPanel::StackBinder ZayPanel::_push_clip_by_rect(const Rect& r, bool doScissor)
    {
        return _push_clip(r.l, r.t, r.r, r.b, doScissor);
    }

    ZayPanel::StackBinder ZayPanel::_push_clip_ui_by_rect(const Rect& r, bool doScissor, chars uiname, SubGestureCB cb, bool hoverpass)
    {
        return _push_clip_ui(r.l, r.t, r.r, r.b, doScissor, uiname, cb, hoverpass);
    }

    ZayPanel::StackBinder ZayPanel::_push_clip_by_child(sint32 ix, sint32 iy, sint32 xcount, sint32 ycount, bool doScissor)
    {
        Rect CalcedRect;
        if(!m_child_image) CalcedRect = m_child_guide;
        else CalcedRect = m_child_image->CalcChildRect(m_child_guide, ix, iy, xcount, ycount);

        const Clip& LastClip = m_stack_clip[-1];
        const float l = CalcedRect.l - LastClip.l;
        const float t = CalcedRect.t - LastClip.t;
        const float r = CalcedRect.r - LastClip.l;
        const float b = CalcedRect.b - LastClip.t;
        return _push_clip(l, t, r, b, doScissor);
    }

    ZayPanel::StackBinder ZayPanel::_push_clip_ui_by_child(sint32 ix, sint32 iy, sint32 xcount, sint32 ycount, bool doScissor, chars uiname, SubGestureCB cb, bool hoverpass)
    {
        if(auto& CurBinder = _push_clip_by_child(ix, iy, xcount, ycount, doScissor))
        {
            if(auto CurCollector = (TouchCollector*) m_ref_touch_collector)
            {
                const Clip& LastClip = m_stack_clip[-1];
                const float& LastZoom = m_stack_zoom[-1];
                CurCollector->mTouchRects.AtAdding() =
                    TouchRect::Create(uiname, LastClip.l, LastClip.t, LastClip.r, LastClip.b, LastZoom, cb, hoverpass);
            }
            else if(auto CurTouch = (ZayView::Touch*) m_ref_touch)
            {
                const Clip& LastClip = m_stack_clip[-1];
                const float& LastZoom = m_stack_zoom[-1];
                CurTouch->update(uiname, LastClip.l, LastClip.t, LastClip.r, LastClip.b, LastZoom, cb, hoverpass);
            }
            return CurBinder;
        }
        return StackBinder(this);
    }

    ZayPanel::StackBinder ZayPanel::_push_color(sint32 r, sint32 g, sint32 b, sint32 a)
    {
        Color& NewColor = m_stack_color.AtAdding();
        const Color& LastColor = m_stack_color[-2];
        NewColor = LastColor * Color(r, g, b, a);

        // 직접 입력방식
        if(r < 0) NewColor.r = -r;
        if(g < 0) NewColor.g = -g;
        if(b < 0) NewColor.b = -b;
        if(a < 0) NewColor.a = -a;

        Platform::Graphics::SetColor(NewColor.r, NewColor.g, NewColor.b, NewColor.a);
        return StackBinder(this, ST_Color);
    }

    ZayPanel::StackBinder ZayPanel::_push_color(const Color& color)
    {
        return _push_color(color.r, color.g, color.b, color.a);
    }

    ZayPanel::StackBinder ZayPanel::_push_color_clear()
    {
        Color& NewColor = m_stack_color.AtAdding();
        NewColor = Color(Color::ColoringDefault);

        Platform::Graphics::SetColor(NewColor.r, NewColor.g, NewColor.b, NewColor.a);
        return StackBinder(this, ST_Color);
    }

    ZayPanel::StackBinder ZayPanel::_push_font(float size, chars name)
    {
        Font& NewFont = m_stack_font.AtAdding();
        const Font& LastFont = m_stack_font[-2];
        NewFont.name = (name)? name : (chars) LastFont.name;
        NewFont.size = LastFont.size * size;

        Platform::Graphics::SetFont(NewFont.name, NewFont.size);
        return StackBinder(this, ST_Font);
    }

    ZayPanel::StackBinder ZayPanel::_push_zoom(float zoom)
    {
        float& NewZoom = m_stack_zoom.AtAdding();
        const float& LastZoom = m_stack_zoom[-2];
        NewZoom = LastZoom * zoom;

        Platform::Graphics::SetZoom(NewZoom);

        Clip& NewClip = m_stack_clip.AtAdding();
        const Clip& LastClip = m_stack_clip[-2];
        NewClip = Clip(LastClip.l / zoom, LastClip.t / zoom, LastClip.r / zoom, LastClip.b / zoom, true);
        m_clipped_width = NewClip.Width();
        m_clipped_height = NewClip.Height();

        Rect& NewRect = m_stack_scissor.AtAdding();
        const Rect& LastRect = m_stack_scissor[-2];
        NewRect = Rect(LastRect.l / zoom, LastRect.t / zoom, LastRect.r / zoom, LastRect.b / zoom);

        Platform::Graphics::SetScissor(NewRect.l, NewRect.t, NewRect.Width(), NewRect.Height());
        return StackBinder(this, ST_Zoom);
    }

    ZayPanel::StackBinder ZayPanel::_push_pass()
    {
        return StackBinder(this, ST_Pass);
    }

    void ZayPanel::_pop_clip()
    {
        BLIK_ASSERT("Pop할 잔여스택이 없습니다", 1 < m_stack_clip.Count());
        if(m_stack_clip[-1].didscissor)
            _pop_scissor();
        m_stack_clip.SubtractionOne();

        const Clip& LastClip = m_stack_clip[-1];
        m_clipped_width = LastClip.Width();
        m_clipped_height = LastClip.Height();
    }

    void ZayPanel::_pop_color()
    {
        BLIK_ASSERT("Pop할 잔여스택이 없습니다", 1 < m_stack_color.Count());
        m_stack_color.SubtractionOne();

        const Color& LastColor = m_stack_color[-1];
        Platform::Graphics::SetColor(LastColor.r, LastColor.g, LastColor.b, LastColor.a);
    }

    void ZayPanel::_pop_font()
    {
        BLIK_ASSERT("Pop할 잔여스택이 없습니다", 1 < m_stack_font.Count());
        m_stack_font.SubtractionOne();

        const Font& LastFont = m_stack_font[-1];
        Platform::Graphics::SetFont(LastFont.name, LastFont.size);
    }

    void ZayPanel::_pop_zoom()
    {
        BLIK_ASSERT("Pop할 잔여스택이 없습니다", 1 < m_stack_zoom.Count());
        m_stack_zoom.SubtractionOne();

        const float& LastZoom = m_stack_zoom[-1];
        Platform::Graphics::SetZoom(LastZoom);

        _pop_clip();
    }

    bool ZayPanel::_push_scissor(float l, float t, float r, float b)
    {
        Rect& NewRect = m_stack_scissor.AtAdding();
        const Rect& LastRect = m_stack_scissor[-2];
        NewRect.l = Math::MaxF(l, LastRect.l);
        NewRect.t = Math::MaxF(t, LastRect.t);
        NewRect.r = Math::MinF(r, LastRect.r);
        NewRect.b = Math::MinF(b, LastRect.b);

        if(m_test_scissor && (NewRect.r <= NewRect.l || NewRect.b <= NewRect.t))
        {
            m_stack_scissor.SubtractionOne();
            return false;
        }

        Platform::Graphics::SetScissor(NewRect.l, NewRect.t, NewRect.Width(), NewRect.Height());
        return true;
    }

    void ZayPanel::_pop_scissor()
    {
        BLIK_ASSERT("Pop할 잔여스택이 없습니다", 1 < m_stack_scissor.Count());
        m_stack_scissor.SubtractionOne();

        const Rect& LastRect = m_stack_scissor[-1];
        Platform::Graphics::SetScissor(LastRect.l, LastRect.t, LastRect.Width(), LastRect.Height());
    }

    class ZayController : public ZayObject
    {
    private:
        ZayController() {}
        ~ZayController() {}

    public:
        void nextFrame()
        {
            m_frame_counter++;
            m_frame_updater.Flush(ZayObject::invalidator, this);
        }

        void wakeUpCheck()
        {
            m_frame_updater.WakeUp(ZayObject::invalidator, this);
        }

        void setCallback(ZayObject::FinderCB fcb, void* fdata, ZayObject::UpdaterCB icb, void* idata)
        {
            m_finder = fcb;
            m_finder_data = fdata;
            m_updater = icb;
            m_updater_data = idata;
        }
    };

    ZayView::ZayView(chars viewclass) : View(),
        m_ref_func(ZayView::_accessfunc(viewclass, false)), m_viewclass((viewclass)? viewclass : "")
    {
        BLIK_ASSERT(String::Format("존재하지 않는 뷰(%s)를 생성하려 합니다", viewclass), m_ref_func);
        m_class = m_ref_func->m_alloc();
        m_touch = Buffer::Alloc<Touch>(BLIK_DBG 1);
        m_agreed_quit = false;
    }

    ZayView::~ZayView()
    {
        m_ref_func->m_free(m_class);
        Buffer::Free(m_touch);
    }

    View* ZayView::Creator(chars viewclass)
    {
        return new ZayView(viewclass);
    }

    h_view ZayView::SetView(h_view view)
    {
        h_view OldViewHandle = m_class->m_view;
        m_class->m_view = view;
        return OldViewHandle;
    }

    bool ZayView::IsNative()
    {
        return m_ref_func->m_isnative;
    }

    void* ZayView::GetClass()
    {
        return m_class;
    }

    void ZayView::SendNotify(chars sender, chars topic, id_share in, id_cloned_share* out)
    {
        m_ref_func->m_bind(m_class);
        m_ref_func->m_notify(sender, topic, in, out);
        m_ref_func->m_bind(nullptr);
    }

    void ZayView::SetCallback(UpdaterCB cb, payload data)
    {
        ((ZayController*) m_class)->setCallback(_finder, this, cb, data);
    }

    void ZayView::OnCreate()
    {
        BLIK_ASSERT("브로드캐스트 등록에 실패하였습니다", m_class);
        View::Search(m_viewclass, SC_Create, m_class->m_view);

        m_ref_func->m_bind(m_class);
        m_ref_func->m_command(CT_Create, "", nullptr, nullptr);
        m_ref_func->m_bind(nullptr);
    }

    bool ZayView::OnCanQuit()
    {
        if(!m_agreed_quit)
        {
            m_ref_func->m_bind(m_class);
            id_cloned_share out = nullptr;
            m_ref_func->m_command(CT_CanQuit, "(Can you quit?)", nullptr, &out);
            m_ref_func->m_bind(nullptr);

            bool Result = true;
            if(out)
            {
                bools OutResult((id_cloned_share_read) out);
                Result = (m_agreed_quit = !!OutResult[0]);
            }
            return Result;
        }
        return m_agreed_quit;
    }

    void ZayView::OnDestroy()
    {
        m_ref_func->m_bind(m_class);
        m_ref_func->m_command(CT_Destroy, "", nullptr, nullptr);
        m_ref_func->m_bind(nullptr);

        BLIK_ASSERT("브로드캐스트 해제에 실패하였습니다", m_class);
        View::Search(m_viewclass, SC_Destroy, m_class->m_view);
    }

    void ZayView::OnSize(sint32 w, sint32 h)
    {
        sint32s WH;
        WH.AtAdding() = w;
        WH.AtAdding() = h;
        m_ref_func->m_bind(m_class);
        m_ref_func->m_command(CT_Size, "", WH, nullptr);
        m_ref_func->m_bind(nullptr);
    }

    void ZayView::OnTick()
    {
        m_ref_func->m_bind(m_class);
        m_ref_func->m_command(CT_Tick, "", nullptr, nullptr);
        m_ref_func->m_bind(nullptr);

        ((ZayController*) m_class)->wakeUpCheck();
    }

    void ZayView::OnRender(sint32 width, sint32 height, float l, float t, float r, float b)
    {
        ZayPanel NewPanel(width, height, m_touch);
        ZAY_LTRB_SCISSOR(NewPanel, l, t, r, b)
        ZAY_XYWH(NewPanel, -l, -t, r, b)
        {
            m_ref_func->m_bind(m_class);
            m_ref_func->m_render(NewPanel);
            m_ref_func->m_bind(nullptr);
        }
        ((ZayController*) m_class)->nextFrame();

        // 매프레임마다 화면갱신을 위한 터치를 일으킴(마우스기기를 위한 시나리오)
        Touch* CurTouch = (Touch*) m_touch;
        if(!CurTouch->ishovered(0)) // 호버ID가 0이 아니라면, 즉 저장된 호버좌표가 있다면
            OnTouch(TT_Render, 0, CurTouch->hoverx(), CurTouch->hovery());
    }

    void ZayView::OnTouch(TouchType type, sint32 id, sint32 x, sint32 y)
    {
        Touch* CurTouch = (Touch*) m_touch;
        const Element& CurElement = CurTouch->get(x, y);
        const Element* PressElement = CurTouch->getpress();
        GestureType SavedType = GT_Null;

        bool NeedUpdate = CurTouch->hovertest(x, y);
        NeedUpdate |= CurTouch->changefocus(&CurElement);
        if(type == TT_Press) NeedUpdate |= CurTouch->changepress(&CurElement);
        else if(type == TT_Release) NeedUpdate |= CurTouch->changepress(nullptr);

        if(type != TT_Render)
        {
            if(CurElement.m_cb)
            {
                m_ref_func->m_bind(m_class);
                const bool IsSameElement = (&CurElement == PressElement);
                switch(type)
                {
                case TT_Moving: CurElement.m_cb(this, &CurElement, SavedType = GT_Moving, x, y); break;
                case TT_Press: CurElement.m_cb(this, &CurElement, SavedType = GT_Pressed, x, y); break;
                case TT_Dragging:
                    BLIK_ASSERT("PressElement가 없습니다", PressElement);
                    if(!IsSameElement && PressElement->m_cb)
                        PressElement->m_cb(this, PressElement, GT_OutDragging, x, y);
                    CurElement.m_cb(this, &CurElement, SavedType = ((IsSameElement)? GT_InDragging : GT_Dropping), x, y);
                    break;
                case TT_Release:
                    BLIK_ASSERT("PressElement가 없습니다", PressElement);
                    CurElement.m_cb(this, &CurElement, SavedType = ((IsSameElement)? GT_InReleased : GT_Dropped), x, y);
                    if(!IsSameElement && PressElement->m_cb)
                        PressElement->m_cb(this, PressElement, GT_OutReleased, x, y);
                    break;
                case TT_WheelUp: case TT_WheelDown:
                case TT_WheelPress: case TT_WheelDragging: case TT_WheelRelease:
                case TT_ExtendPress: case TT_ExtendDragging: case TT_ExtendRelease:
                    CurElement.m_cb(this, &CurElement, (GestureType) (type - TT_WheelUp + GT_WheelUp), x, y);
                    if(&CurElement != CurTouch->get())
                        CurTouch->get()->m_cb(this, CurTouch->get(), (GestureType) (type - TT_WheelUp + GT_WheelUpPeeked), x, y);
                    break;
                case TT_ToolTip:
                    CurElement.m_cb(this, &CurElement, GT_ToolTip, x, y);
                    break;
                case TT_LongPress:
                    CurElement.m_cb(this, &CurElement, GT_LongPressed, x, y);
                    break;
                }
                m_ref_func->m_bind(nullptr);
            }

            if(SavedType != GT_Null)
            {
                if(const Element* OldMover = CurTouch->changemoving(&CurElement, SavedType))
                {
                    if(OldMover->m_cb)
                    {
                        m_ref_func->m_bind(m_class);
                        OldMover->m_cb(this, OldMover, GT_MovingLosed, x, y);
                        m_ref_func->m_bind(nullptr);
                    }
                }

                if(const Element* OldDropper = CurTouch->changedropping(&CurElement, SavedType))
                {
                    if(OldDropper->m_cb)
                    {
                        m_ref_func->m_bind(m_class);
                        OldDropper->m_cb(this, OldDropper, GT_DroppingLosed, x, y);
                        m_ref_func->m_bind(nullptr);
                    }
                }
            }
        }

        if(NeedUpdate)
            m_class->invalidate();
    }

    void ZayView::OnKey(sint32 code, chars text, bool pressed)
    {
        m_ref_func->m_bind(m_class);
        pointers in;
        in.AtAdding() = (void*) &code;
        in.AtAdding() = (void*) text;
        m_ref_func->m_command(CT_Signal, (pressed)? "KeyPress" : "KeyRelease", in, nullptr);
        m_ref_func->m_bind(nullptr);
    }

    void ZayView::_gesture(GestureType type, sint32 x, sint32 y)
    {
        m_ref_func->m_bind(m_class);
        m_ref_func->m_gesture(type, x, y);
        m_ref_func->m_bind(nullptr);
    }

    autorun ZayView::_makefunc(bool isnative, chars viewclass,
        ZayObject::CommandCB c, ZayObject::NotifyCB n, ZayPanel::GestureCB g, ZayPanel::RenderCB r,
        ZayObject::BindCB b, ZayObject::AllocCB a, ZayObject::FreeCB f)
    {
        BLIK_ASSERT("중복된 이름의 뷰가 존재합니다", !_accessfunc(viewclass, false));
        Function* NewFunction = _accessfunc(viewclass, true);
        NewFunction->m_isnative = isnative;
        NewFunction->m_viewclass = viewclass;
        NewFunction->m_command = c;
        NewFunction->m_notify = n;
        NewFunction->m_gesture = g;
        NewFunction->m_render = r;
        NewFunction->m_bind = b;
        NewFunction->m_alloc = a;
        NewFunction->m_free = f;
        return true;
    }

    ZayView::Function* ZayView::_accessfunc(chars viewclass, bool creatable)
    {
        static Map<Function> AllFunctions;
        // 본 함수는 아래와 같은 TLS기법으로 AllFunctions의 싱글톤을 관리하지 않는다.
        // "Map<Function>& AllFunctions = *BLIK_STORAGE_SYS(Map<Function>);"
        // 그 이유는 안드로이드 DLL인 so와 같이 main()진입전의 전역변수등의 초기화수행시
        // main스레드가 아닌 곳에서 수행하기 때문이다.
        // 즉, BLIK_DECLARE_VIEW_CLASS(...)으로 뷰를 자동등록하는 과정의
        // 시나리오에 혼선을 야기하는 결과를 초래하기 때문이다. 또한 안드로이드 디버깅의 경우,
        // main()진입전에 일어난 초기화수행의 과정은 브레이크조차 잡히지 않는다.

        chars ViewName = (viewclass && *viewclass)? viewclass : "_defaultview_";
        return (creatable)? &AllFunctions(ViewName) : AllFunctions.Access(ViewName);
    }

    const void* ZayView::_finder(void* data, chars uiname)
    {
        Touch* CurTouch = (Touch*) ((ZayView*) data)->m_touch;
        return CurTouch->get(uiname, 0);
    }

    ZayView::Element::Element()
    {
        m_updateid = -1;
        m_rect.l = 0;
        m_rect.t = 0;
        m_rect.r = 0;
        m_rect.b = 0;
        m_zoom = 1;
        m_cb = nullptr;
        m_subcb = nullptr;
        m_hoverpass = false;
        m_hoverid = -1;
        m_saved_xy.x = 0;
        m_saved_xy.y = 0;
        m_saved_updateid_for_state = -1;
        m_saved_state = PS_Null;
        m_saved_state_old = PS_Null;
    }

    ZayView::Element::~Element()
    {
    }

    ZayView::Element& ZayView::Element::operator=(const Element& rhs)
    {
        m_updateid = rhs.m_updateid;
        m_name = rhs.m_name;
        m_rect.l = rhs.m_rect.l;
        m_rect.t = rhs.m_rect.t;
        m_rect.r = rhs.m_rect.r;
        m_rect.b = rhs.m_rect.b;
        m_zoom = rhs.m_zoom;
        m_cb = rhs.m_cb;
        m_subcb = rhs.m_subcb;
        m_hoverpass = rhs.m_hoverpass;
        m_hoverid = rhs.m_hoverid;
        m_saved_xy.x = rhs.m_saved_xy.x;
        m_saved_xy.y = rhs.m_saved_xy.y;
        m_saved_updateid_for_state = rhs.m_saved_updateid_for_state;
        m_saved_state = rhs.m_saved_state;
        m_saved_state_old = rhs.m_saved_state_old;
        return *this;
    }

    PanelState ZayView::Element::GetState(void* touch) const
    {
        if(m_updateid != m_saved_updateid_for_state)
        {
            PanelState StateCollector = PS_Null;
            auto CurTouch = (const ZayView::Touch*) touch;
            if(CurTouch->ishovered(m_hoverid))
                StateCollector = StateCollector | PS_Hovered;
            if(CurTouch->getfocus() == this)
            {
                StateCollector = StateCollector | PS_Focused;
                if(CurTouch->getpress())
                {
                    if(CurTouch->getpress() == this)
                        StateCollector = StateCollector | PS_Pressed;
                    else StateCollector = StateCollector | PS_Dropping;
                }
            }
            if(CurTouch->getpress() == this)
                StateCollector = StateCollector | PS_Dragging;
            // 처리결과를 캐시에 저장
            m_saved_updateid_for_state = m_updateid;
            m_saved_state_old = m_saved_state;
            m_saved_state = StateCollector;
        }
        return m_saved_state;
    }

    bool ZayView::Element::IsStateChanged(void* touch) const
    {
        return (GetState(touch) != m_saved_state_old);
    }

    ZayView::Touch::Touch()
    {
        m_updateid = 0;
        m_hoverid = 0;
        m_block_width = 0;
        m_block_height = 0;
        m_focus = nullptr;
        m_press = nullptr;
        m_moving = nullptr;
        m_dropping = nullptr;
        m_hover_x = -1;
        m_hover_y = -1;
    }

    ZayView::Touch::~Touch()
    {
    }

    ZayView::Touch& ZayView::Touch::operator=(const ZayView::Touch& rhs)
    {
        m_updateid = rhs.m_updateid;
        m_hoverid = rhs.m_hoverid;
        m_block_width = rhs.m_block_width;
        m_block_height = rhs.m_block_height;
        m_element = rhs.m_element;
        m_map = rhs.m_map;
        m_cell = rhs.m_cell;
        m_focus = rhs.m_focus;
        m_press = rhs.m_press;
        m_moving = rhs.m_moving;
        m_dropping = rhs.m_dropping;
        m_hover_x = rhs.m_hover_x;
        m_hover_y = rhs.m_hover_y;
        return *this;
    }

    void ZayView::Touch::ready(sint32 width, sint32 height)
    {
        m_updateid++;
        m_block_width = (width + Cell::Size - 1) / Cell::Size;
        m_block_height = (height + Cell::Size - 1) / Cell::Size;
        m_element.m_rect.l = 0;
        m_element.m_rect.t = 0;
        m_element.m_rect.r = width;
        m_element.m_rect.b = height;
        m_element.m_cb = OnGesture;
    }

    void ZayView::Touch::update(chars uiname, float l, float t, float r, float b,
        float zoom, ZayPanel::SubGestureCB cb, bool hoverpass, bool* dirtytest)
    {
        if(uiname == nullptr || uiname[0] == '\0' || r <= l || b <= t)
            return;

        Element& CurElement = m_map(uiname);
        BLIK_ASSERT("동일한 uiname이 여러번 사용되고 있습니다", CurElement.m_updateid < m_updateid);
        CurElement.m_updateid = m_updateid;
        CurElement.m_name = uiname;
        CurElement.m_rect.l = (sint32) (l * zoom);
        CurElement.m_rect.t = (sint32) (t * zoom);
        CurElement.m_rect.r = (sint32) (r * zoom);
        CurElement.m_rect.b = (sint32) (b * zoom);
        CurElement.m_zoom = zoom;
        CurElement.m_cb = OnSubGesture;
        CurElement.m_subcb = cb;
        CurElement.m_hoverpass = hoverpass;

        const sint32 CellL = Math::Max(0, CurElement.m_rect.l / Cell::Size);
        const sint32 CellT = Math::Max(0, CurElement.m_rect.t / Cell::Size);
        const sint32 CellR = Math::Min(m_block_width, (CurElement.m_rect.r + Cell::Size - 1) / Cell::Size);
        const sint32 CellB = Math::Min(m_block_height, (CurElement.m_rect.b + Cell::Size - 1) / Cell::Size);
        for(sint32 y = CellT; y < CellB; ++y)
        for(sint32 x = CellL; x < CellR; ++x)
        {
            Cell& CurCell = m_cell.AtWherever(y).AtWherever(x);
            if(CurCell.m_updateid != m_updateid)
            {
                CurCell.m_updateid = m_updateid;
                CurCell.m_validlength = 0;
            }
            CurCell.m_elements.AtWherever(CurCell.m_validlength++) = &CurElement;
        }

        if(dirtytest && !*dirtytest)
            *dirtytest = CurElement.IsStateChanged(this);
    }

    const ZayView::Element* ZayView::Touch::get() const
    {
        return &m_element;
    }

    const ZayView::Element* ZayView::Touch::get(chars uiname, sint32 lag) const
    {
        BLIK_ASSERT("uiname이 nullptr입니다", uiname);
        if(const Element* CurElement = m_map.Access(uiname))
        if(m_updateid <= CurElement->m_updateid + lag)
            return CurElement;
        return nullptr;
    }

    const ZayView::Element& ZayView::Touch::get(sint32 x, sint32 y) const
    {
        if(const Cell* CurCell = getcell_const(x, y))
        {
            for(sint32 i = CurCell->m_validlength - 1; 0 <= i; --i)
            {
                const Element* CurElement = CurCell->m_elements[i];
                const rect128& CurRect = CurElement->m_rect;
                if(CurRect.l <= x && CurRect.t <= y && x < CurRect.r && y < CurRect.b)
                    return *CurElement;
            }
        }
        return m_element;
    }

    bool ZayView::Touch::hovertest(sint32 x, sint32 y)
    {
        m_hoverid++;
        bool NeedUpdate = false;

        Cell* CurCell = getcell(x, y);
        if(CurCell)
        for(sint32 i = CurCell->m_validlength - 1; 0 <= i; --i)
        {
            Element* CurElement = CurCell->m_elements.At(i);
            const rect128& CurRect = CurElement->m_rect;
            if(CurRect.l <= x && CurRect.t <= y && x < CurRect.r && y < CurRect.b)
            {
                NeedUpdate |= (CurElement->m_hoverid < m_hoverid - 1);
                CurElement->m_hoverid = m_hoverid;
                if(!CurElement->m_hoverpass) break;
            }
        }

        if(!NeedUpdate)
        {
            const Cell* OldCell = CurCell;
            if(x / Cell::Size != m_hover_x / Cell::Size || y / Cell::Size != m_hover_y / Cell::Size)
                OldCell = getcell_const(m_hover_x, m_hover_y);
            if(OldCell)
            for(sint32 i = 0; i < OldCell->m_validlength; ++i)
            {
                const Element* CurElement = OldCell->m_elements[i];
                const rect128& CurRect = CurElement->m_rect;
                if(CurRect.l <= m_hover_x && CurRect.t <= m_hover_y && m_hover_x < CurRect.r && m_hover_y < CurRect.b)
                {
                    NeedUpdate |= (CurElement->m_hoverid == m_hoverid - 1);
                    if(NeedUpdate) break;
                }
            }
        }

        m_hover_x = x;
        m_hover_y = y;
        return NeedUpdate;
    }

    ZayView::Touch::Cell* ZayView::Touch::getcell(sint32 x, sint32 y)
    {
        const sint32 CellX = x / Cell::Size;
        const sint32 CellY = y / Cell::Size;
        if(0 <= CellX && 0 <= CellY && CellX < m_block_width && CellY < m_block_height)
        {
            if(CellY < m_cell.Count())
            {
                CellRow& CurCellRow = m_cell.At(CellY);
                if(CellX < CurCellRow.Count())
                {
                    Cell& CurCell = CurCellRow.At(CellX);
                    if(CurCell.m_updateid == m_updateid)
                        return &CurCell;
                }
            }
        }
        return nullptr;
    }

    const ZayView::Touch::Cell* ZayView::Touch::getcell_const(sint32 x, sint32 y) const
    {
        const sint32 CellX = x / Cell::Size;
        const sint32 CellY = y / Cell::Size;
        if(0 <= CellX && 0 <= CellY && CellX < m_block_width && CellY < m_block_height)
        {
            if(CellY < m_cell.Count())
            {
                const CellRow& CurCellRow = m_cell[CellY];
                if(CellX < CurCellRow.Count())
                {
                    const Cell& CurCell = CurCellRow[CellX];
                    if(CurCell.m_updateid == m_updateid)
                        return &CurCell;
                }
            }
        }
        return nullptr;
    }

    void ZayView::Touch::OnGesture(ZayView* manager, const Element* data, GestureType type, sint32 x, sint32 y)
    {
        manager->_gesture(type, x, y);
        data->m_saved_xy.x = x;
        data->m_saved_xy.y = y;
    }

    void ZayView::Touch::OnSubGesture(ZayView* manager, const Element* data, GestureType type, sint32 x, sint32 y)
    {
        if(data->m_subcb)
        {
            data->m_subcb((ZayObject*) manager->GetClass(), data->m_name, type, x, y);
            data->m_saved_xy.x = x;
            data->m_saved_xy.y = y;
        }
    }

    ZayView::Function::Function()
    {
        m_isnative = false;
        m_command = nullptr;
        m_notify = nullptr;
        m_gesture = nullptr;
        m_render = nullptr;
        m_bind = nullptr;
        m_alloc = nullptr;
        m_free = nullptr;
    }

    ZayView::Function::~Function()
    {
    }
}
