#pragma once
#include <platform/blik_platform.hpp>
#include <platform/blik_platform_impl.hpp>

#include <addon/blik_addon.hpp>

#ifdef BLIK_PLATFORM_QT5

    #include <element/blik_image.hpp>
    #include <format/blik_bmp.hpp>

    #include <QtWidgets>
    #include <QMainWindow>
    #include <QHostInfo>
    #include <QTcpSocket>
    #include <QUdpSocket>
    #include <QTcpServer>

    #include <QGLWidget>
    #include <QGLFunctions>
    #include <QGLShaderProgram>

    #include <QtSerialPort>
    #include <QSerialPortInfo>

    #include <QCamera>
    #include <QCameraInfo>
    #include <QCameraImageCapture>
    #include <QAbstractVideoSurface>

    #include <QAudioRecorder>
    #include <QAudioProbe>
    #include <QAudioDeviceInfo>

    #include <QWebEngineView>

    #if BLIK_ANDROID
        #include <QtAndroidExtras/QAndroidJniObject>
        #include <QtAndroidExtras/QAndroidJniEnvironment>
        #include <jni.h>
        #include <termios.h>
        #include <unistd.h>
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <fcntl.h>
        #include <errno.h>
        #include <string.h>
    #endif

    class MainData;
    class QMainWindow;
    extern MainData* g_data;
    extern QMainWindow* g_window;
    extern QWidget* g_view;
    extern QGLFunctions* g_func;

    class SizePolicy
    {
    public:
        SizePolicy();
        ~SizePolicy();
    public:
        sint32 m_minwidth;
        sint32 m_minheight;
        sint32 m_maxwidth;
        sint32 m_maxheight;
    };

    class CanvasClass
    {
    public:
        CanvasClass();
        CanvasClass(QPaintDevice* device);
        ~CanvasClass();
    public:
        void Bind(QPaintDevice* device);
        void Unbind();
    private:
        void BindCore(QPaintDevice* device);
        void UnbindCore();
    public:
        static inline CanvasClass* get() {return ST();}
        inline QPainter& painter() {return mPainter;}
        inline const QColor& color() const {return mColor;}
        inline void SetColor(uint08 r, uint08 g, uint08 b, uint08 a)
        {mColor.setRgb(r, g, b, a);}
    private:
        static inline CanvasClass*& ST() {static CanvasClass* _ = nullptr; return _;}
    private:
        const bool mIsTypeSurface;
        bool mIsSurfaceBinded;
        CanvasClass* mSavedCanvas;
        QRectF mSavedClipRect;
        QPainter mPainter;
        QColor mColor;
    };

    class ViewAPI : public QObject
    {
        Q_OBJECT

    public:
        enum ParentType {PT_Null, PT_GenericView, PT_MainViewGL, PT_MainViewMDI};

    public:
        ViewAPI(ParentType type, buffer buf, View* manager, View::UpdaterCB cb, QWidget* data, QWidget* device)
        {
            m_parent_type = type;
            m_parent_buf = buf;
            m_parent_ptr = nullptr;
            m_view_manager = manager;
            m_next_manager = nullptr;
            m_view_cb = cb;
            m_view_data = data;
            m_paint_device = device;
            if(manager)
                manager->SetCallback(m_view_cb, m_view_data);

            m_input_enabled = true;
            m_width = 0;
            m_height = 0;
            m_exiting = false;
            m_paintcount = 0;
            m_tooltip_x = 0;
            m_tooltip_y = 0;
            m_longpress_x = 0;
            m_longpress_y = 0;

            connect(&m_tick_timer, &QTimer::timeout, this, &ViewAPI::tick_timeout);
            connect(&m_update_timer, &QTimer::timeout, this, &ViewAPI::update_timeout);
            connect(&m_tooltip_timer, &QTimer::timeout, this, &ViewAPI::tooltip_timeout);
            connect(&m_longpress_timer, &QTimer::timeout, this, &ViewAPI::longpress_timeout);
        }

        ViewAPI(const ViewAPI& rhs) = delete;
        ViewAPI& operator=(const ViewAPI& rhs) = delete;

        ~ViewAPI()
        {
            m_tick_timer.stop();
            m_update_timer.stop();
            m_tooltip_timer.stop();
            m_longpress_timer.stop();

            if(getParent()) sendDestroy();
            Buffer::Free(m_parent_buf);
            delete m_view_manager;
            delete m_next_manager;
        }

    public:
        inline ParentType getParentType() const
        {
            return m_parent_type;
        }

        inline void* getParent() const
        {
            BLIK_ASSERT("잘못된 시나리오입니다", (m_parent_buf != nullptr) != (m_parent_ptr != nullptr));
            return (void*) (((ublock) m_parent_buf) | ((ublock) m_parent_ptr));
        }

        inline void renewParent(void* ptr)
        {
            m_parent_buf = nullptr;
            m_parent_ptr = ptr;

            if(getParent())
                sendCreate();
        }

        inline bool parentIsPtr() const
        {
            return (m_parent_ptr != nullptr);
        }

        inline bool hasViewManager() const
        {
            return (m_view_manager != nullptr);
        }

        void setViewAndCreate(h_view view)
        {
            BLIK_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->SetView(view);

            sendCreate();
        }

        h_view changeViewManagerAndDestroy(View* manager)
        {
            sendDestroy();
            h_view OldViewHandle;
            if(m_view_manager)
            {
                OldViewHandle = m_view_manager->SetView(h_view::null());
                delete m_view_manager;
            }

            m_view_manager = manager;
            m_view_manager->SetCallback(m_view_cb, m_view_data);
            return OldViewHandle;
        }

        inline void setNextViewManager(View* manager)
        {
            delete m_next_manager;
            m_next_manager = manager;
        }

        inline void changeViewData(QWidget* data)
        {
            m_view_data = data;
            if(m_view_manager)
                m_view_manager->SetCallback(m_view_cb, m_view_data);
        }

        inline ViewClass* getClass() const
        {
            return (ViewClass*) m_view_manager->GetClass();
        }

        inline QWidget* getWidget() const
        {
            if(m_view_manager)
            {
                if(m_view_data)
                    return m_view_data;
                if(m_view_manager->IsNative())
                    return (QWidget*) m_view_manager->GetClass();
            }
            return (QWidget*) getParent();
        }

        inline QWidget* getWidgetForPaint() const
        {
            if(m_paint_device)
                return m_paint_device;
            return getWidget();
        }

        inline void render(sint32 width, sint32 height)
        {
            BLIK_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->OnRender(width, height, 0, 0, width, height);

            if(m_next_manager)
            {
                h_view OldViewHandle = changeViewManagerAndDestroy(m_next_manager);
                setViewAndCreate(OldViewHandle);
                m_next_manager = nullptr;
            }
        }

        inline void touch(TouchType type, sint32 id, sint32 x, sint32 y)
        {
            BLIK_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->OnTouch(type, id, x, y);
        }

        inline void key(sint32 code, chars text, bool pressed)
        {
            BLIK_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->OnKey(code, text, pressed);
        }

        inline void sendCreate()
        {
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                m_view_manager->OnCreate();
            }
            m_tick_timer.start(17);
        }

        inline bool canQuit()
        {
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                return m_view_manager->OnCanQuit();
            }
            return true;
        }

        inline void sendDestroy()
        {
            m_tick_timer.stop();
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                m_view_manager->OnDestroy();
            }
        }

        inline void sendTick() const
        {
            if(m_view_manager != nullptr)
            {
                g_view = getWidget();
                m_view_manager->OnTick();
            }
        }

        inline void sendNotify(chars topic, id_share in, id_cloned_share* out) const
        {
            BLIK_ASSERT("잘못된 시나리오입니다", m_view_manager);
            g_view = getWidget();
            m_view_manager->SendNotify("(platform)", topic, in, out);
        }

        inline bool isInputEnabled() const
        {
            return m_input_enabled;
        }

        inline void setInputEnabled(bool enable)
        {
            m_input_enabled = enable;
        }

    public:
        void resize(sint32 width, sint32 height)
        {
            m_width = width;
            m_height = height;
            m_view_manager->OnSize(width, height);
        }

        void paint()
        {
            // for assert dialog's recursive call
            if(CanvasClass::get()) return;
            CanvasClass Canvas(getWidgetForPaint());
            render(m_width, m_height);
        }

        void nextPaint()
        {
            if(0 < m_paintcount)
                m_paintcount--;
        }

        void update(sint32 count)
        {
            if(count == -1)
                m_exiting = true;
            else if(m_paintcount < count)
            {
                if(m_paintcount == 0)
                    m_update_timer.start(17);
                m_paintcount = count;
            }
        }

        bool closeEvent(QCloseEvent* event)
        {
            if(canQuit())
            {
                event->accept();
                return true;
            }
            event->ignore();
            return false;
        }

        void mousePressEvent(QMouseEvent* event)
        {
            if(!m_input_enabled)
            {
                event->ignore();
                return;
            }

            if(event->button() == Qt::LeftButton)
            {
                touch(TT_Press, 0, event->x(), event->y());
                m_longpress_timer.start(3000);
                m_longpress_x = event->x();
                m_longpress_y = event->y();
            }
            else if(event->button() == Qt::RightButton)
                touch(TT_ExtendPress, 0, event->x(), event->y());
            else if(event->button() == Qt::MidButton)
                touch(TT_WheelPress, 0, event->x(), event->y());

            m_tooltip_timer.stop();
        }

        void mouseMoveEvent(QMouseEvent* event)
        {
            if(!m_input_enabled)
            {
                event->ignore();
                return;
            }

            if(event->buttons() == Qt::NoButton)
            {
                touch(TT_Moving, 0, event->x(), event->y());
                m_tooltip_timer.start(300);
                m_tooltip_x = event->x();
                m_tooltip_y = event->y();
                Platform::Popup::HideToolTip();
            }
            else
            {
                if(event->buttons() & Qt::LeftButton)
                    touch(TT_Dragging, 0, event->x(), event->y());
                if(event->buttons() & Qt::RightButton)
                    touch(TT_ExtendDragging, 0, event->x(), event->y());
                if(event->buttons() & Qt::MidButton)
                    touch(TT_WheelDragging, 0, event->x(), event->y());
            }

            if(5 < Math::Distance(m_longpress_x, m_longpress_y, event->x(), event->y()))
                m_longpress_timer.stop();
        }

        void mouseReleaseEvent(QMouseEvent* event)
        {
            if(!m_input_enabled)
            {
                event->ignore();
                return;
            }

            if(event->button() == Qt::LeftButton)
                touch(TT_Release, 0, event->x(), event->y());
            else if(event->button() == Qt::RightButton)
                touch(TT_ExtendRelease, 0, event->x(), event->y());
            else if(event->button() == Qt::MidButton)
                touch(TT_WheelRelease, 0, event->x(), event->y());

            m_longpress_timer.stop();
        }

        void wheelEvent(QWheelEvent* event)
        {
            if(!m_input_enabled)
            {
                event->ignore();
                return;
            }

            sint32 WheelValue = event->delta() / 120;
            while(0 < WheelValue)
            {
                WheelValue--;
                touch(TT_WheelUp, 0, event->x(), event->y());
            }
            while(WheelValue < 0)
            {
                WheelValue++;
                touch(TT_WheelDown, 0, event->x(), event->y());
            }
        }

        void keyPressEvent(QKeyEvent* event)
        {
            if(!m_input_enabled)
            {
                event->ignore();
                return;
            }
            key(event->key(), event->text().toUtf8().constData(), true);
        }

        void keyReleaseEvent(QKeyEvent* event)
        {
            if(!m_input_enabled)
            {
                event->ignore();
                return;
            }
            key(event->key(), event->text().toUtf8().constData(), false);
        }

    private:
        void tick_timeout()
        {
            if(m_exiting)
                getWidget()->close();
            else sendTick();
        }

        void update_timeout()
        {
            if(m_paintcount == 0)
                m_update_timer.stop();
            getWidgetForPaint()->update();
        }

        void tooltip_timeout()
        {
            m_tooltip_timer.stop();
            touch(TT_ToolTip, 0, m_tooltip_x, m_tooltip_y);
        }

        void longpress_timeout()
        {
            m_longpress_timer.stop();
            touch(TT_LongPress, 0, m_longpress_x, m_longpress_y);
        }

    private:
        ParentType m_parent_type;
        buffer m_parent_buf;
        void* m_parent_ptr;
        View* m_view_manager;
        View* m_next_manager;
        View::UpdaterCB m_view_cb;
        QWidget* m_view_data;
        QWidget* m_paint_device;

    private:
        bool m_input_enabled;
        sint32 m_width;
        sint32 m_height;
        sint32 m_exiting;
        sint32 m_paintcount;
        sint32 m_tooltip_x;
        sint32 m_tooltip_y;
        sint32 m_longpress_x;
        sint32 m_longpress_y;
        QTimer m_tick_timer;
        QTimer m_update_timer;
        QTimer m_tooltip_timer;
        QTimer m_longpress_timer;
    };

    class GenericView : public QFrame
    {
        Q_OBJECT

    public:
        GenericView(QWidget* parent = nullptr) : QFrame(parent)
        {
            m_api = nullptr;
            setMouseTracking(true);
            setFocusPolicy(Qt::ClickFocus);
            setAutoFillBackground(false);
        }

        GenericView(View* manager, QString name, sint32 width, sint32 height, SizePolicy* policy)
        {
            BLIK_DECLARE_BUFFERED_CLASS(BufferedViewAPI, ViewAPI, PT_Null, nullptr, nullptr, nullptr, nullptr, nullptr);
            buffer NewAPI = Buffer::AllocNoConstructorOnce<BufferedViewAPI>(BLIK_DBG 1);
            BLIK_CONSTRUCTOR(NewAPI, 0, ViewAPI, ViewAPI::PT_GenericView, (buffer) this, manager, updater, this, nullptr);
            m_api = (ViewAPI*) NewAPI;

            m_name = name;
            m_firstwidth = width;
            m_firstheight = height;
            setMinimumSize(policy->m_minwidth, policy->m_minheight);
            setMaximumSize(policy->m_maxwidth, policy->m_maxheight);

            setMouseTracking(true);
            setFocusPolicy(Qt::ClickFocus);
            setAutoFillBackground(false);
        }

        GenericView(h_view view)
        {
            takeView(view);
            setMouseTracking(true);
            setFocusPolicy(Qt::ClickFocus);
            setAutoFillBackground(false);
        }

        virtual ~GenericView()
        {
            if(m_api && m_api->parentIsPtr())
                Buffer::Free((buffer) m_api);
            m_window.set_buf(nullptr);
        }

        GenericView(const GenericView* rhs) {BLIK_ASSERT("사용금지", false);}
        GenericView& operator=(const GenericView& rhs) {BLIK_ASSERT("사용금지", false); return *this;}

    protected:
        void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE
        {
            m_api->resize(event->size().width(), event->size().height());
        }

        void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE
        {
            if(m_api->hasViewManager())
                m_api->paint();
            else QFrame::paintEvent(event);
            m_api->nextPaint();
        }

    protected:
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE
        {
            if(m_api->closeEvent(event))
                m_window.set_buf(nullptr);
        }
        void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseMoveEvent(event);}
        void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseReleaseEvent(event);}
        void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE {m_api->wheelEvent(event);}
        void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyPressEvent(event);}
        void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyReleaseEvent(event);}
        static void updater(void* data, sint32 count)
        {((GenericView*) data)->m_api->update(count);}

    protected:
        void takeView(h_view view)
        {
            GenericView* OldGenericView = cast(view);
            m_api = OldGenericView->m_api;
            OldGenericView->m_api = nullptr;
            m_name = OldGenericView->m_name;
            m_firstwidth = OldGenericView->m_firstwidth;
            m_firstheight = OldGenericView->m_firstheight;
            setMinimumSize(OldGenericView->minimumWidth(), OldGenericView->minimumHeight());
            setMaximumSize(OldGenericView->maximumWidth(), OldGenericView->maximumHeight());
            Buffer::Free((buffer) OldGenericView);

            m_api->renewParent(this);
            m_api->changeViewData(this);
            view.clear_buf();
            view.set_ptr(m_api); // 삭제책임은 GenericView에 이관하였기에
        }

    public:
        inline const QString getName() const
        {
            return m_name;
        }

        inline const QSize getFirstSize() const
        {
            return QSize(m_firstwidth, m_firstheight);
        }

        inline void attachWindow(h_window window)
        {
            m_window = window;
        }

    public:
        static GenericView* cast(h_view view)
        {
            ViewAPI* CurViewAPI = (ViewAPI*) view.get();
            BLIK_ASSERT("타입정보가 일치하지 않습니다", CurViewAPI->getParentType() == ViewAPI::PT_GenericView);
            return (GenericView*) CurViewAPI->getParent();
        }

    public:
        static bool CloseAllWindows()
        {
            QWindowList processedWindows;
            while(auto w = QApplication::activeModalWidget())
            {
                if(QWindow* window = w->windowHandle())
                {
                    if(!window->close()) return false;
                    processedWindows.append(window);
                }
            }
            for(auto w : QApplication::topLevelWidgets())
            {
                if(w->windowType() == Qt::Desktop)
                    continue;
                if(QWindow* window = w->windowHandle())
                {
                    if(!window->close()) return false;
                    processedWindows.append(window);
                }
            }
            return true;
        }

    public:
        ViewAPI* m_api;

    private:
        QString m_name;
        sint32 m_firstwidth;
        sint32 m_firstheight;
        h_window m_window;
    };

    class MainViewGL : public QGLWidget
    {
        Q_OBJECT

    public:
        MainViewGL(QWidget* parent) : QGLWidget(parent)
        {
            BLIK_DECLARE_BUFFERED_CLASS(BufferedViewAPI, ViewAPI, PT_Null, nullptr, nullptr, nullptr, nullptr, nullptr);
            buffer NewAPI = Buffer::AllocNoConstructorOnce<BufferedViewAPI>(BLIK_DBG 1);
            BLIK_CONSTRUCTOR(NewAPI, 0, ViewAPI, ViewAPI::PT_MainViewGL, (buffer) this, nullptr, updater, this, nullptr);
            m_api = (ViewAPI*) NewAPI;

            m_fbo_update = true;
            m_fbo = 0;
            m_fbo_render = 0;
            m_fbo_width = 0;
            m_fbo_height = 0;

            setAttribute(Qt::WA_PaintOnScreen);
            setAttribute(Qt::WA_NoSystemBackground);
            setAttribute(Qt::WA_AcceptTouchEvents);
            setMouseTracking(true);
            setFocusPolicy(Qt::ClickFocus);
            setAutoBufferSwap(false);
            setAutoFillBackground(false);
        }

        virtual ~MainViewGL()
        {
            if(m_api->parentIsPtr())
                Buffer::Free((buffer) m_api);

            if(g_func)
            {
                g_func->glDeleteFramebuffers(1, &m_fbo);
                g_func->glDeleteRenderbuffers(1, &m_fbo_render);
                delete g_func;
                g_func = nullptr;
            }
        }

        MainViewGL(const MainViewGL* rhs) {BLIK_ASSERT("사용금지", false);}
        MainViewGL& operator=(const MainViewGL& rhs) {BLIK_ASSERT("사용금지", false); return *this;}

    public:
        void onCloseEvent(QCloseEvent* event)
        {
            closeEvent(event);
            GenericView::CloseAllWindows();
        }

    protected:
        void initializeGL() Q_DECL_OVERRIDE
        {
            if(!g_func)
                g_func = new QGLFunctions(context());
        }

        void resizeGL(int width, int height) Q_DECL_OVERRIDE
        {
            m_fbo_update = true;
            m_fbo_width = geometry().width();
            m_fbo_height = geometry().height();
            m_api->resize(m_fbo_width, m_fbo_height);
        }

        void paintGL() Q_DECL_OVERRIDE
        {
            if(m_api->hasViewManager())
            {
                if(m_fbo_update)
                {
                    m_fbo_update = false;
                    if(m_fbo) g_func->glDeleteFramebuffers(1, &m_fbo);
                    if(m_fbo_render) g_func->glDeleteRenderbuffers(1, &m_fbo_render);

                    g_func->glGenRenderbuffers(1, &m_fbo_render);
                    g_func->glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_render);
                    g_func->glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, m_fbo_width, m_fbo_height);
                    g_func->glBindRenderbuffer(GL_RENDERBUFFER, 0);

                    g_func->glGenFramebuffers(1, &m_fbo);
                    g_func->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
                    g_func->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_fbo_render);
                    GLenum status = g_func->glCheckFramebufferStatus(GL_FRAMEBUFFER);
                    BLIK_ASSERT("프레임버퍼의 생성에 실패하였습니다", status == GL_FRAMEBUFFER_COMPLETE);
                    g_func->glBindFramebuffer(GL_FRAMEBUFFER, 0);
                }

                m_api->paint();
                swapBuffers();
            }
            else QGLWidget::paintGL();
            m_api->nextPaint();
        }

    protected:
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE {m_api->closeEvent(event);}
        void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseMoveEvent(event);}
        void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseReleaseEvent(event);}
        void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE {m_api->wheelEvent(event);}
        void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyPressEvent(event);}
        void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyReleaseEvent(event);}
        static void updater(void* data, sint32 count)
        {((MainViewGL*) data)->m_api->update(count);}

    public:
        ViewAPI* m_api;

    private:
        bool m_fbo_update;
        GLuint m_fbo;
        GLuint m_fbo_render;
        sint32 m_fbo_width;
        sint32 m_fbo_height;
    };

    class MainViewMDI : public QMdiArea
    {
        Q_OBJECT

    public:
        MainViewMDI(QWidget* parent) : QMdiArea(parent)
        {
            BLIK_DECLARE_BUFFERED_CLASS(BufferedViewAPI, ViewAPI, PT_Null, nullptr, nullptr, nullptr, nullptr, nullptr);
            buffer NewAPI = Buffer::AllocNoConstructorOnce<BufferedViewAPI>(BLIK_DBG 1);
            BLIK_CONSTRUCTOR(NewAPI, 0, ViewAPI, ViewAPI::PT_MainViewMDI, (buffer) this, nullptr, updater, this, viewport());
            m_api = (ViewAPI*) NewAPI;

            m_width = 0;
            m_height = 0;

            setAttribute(Qt::WA_PaintOnScreen);
            setAttribute(Qt::WA_NoSystemBackground);
            setAttribute(Qt::WA_AcceptTouchEvents);
            setMouseTracking(true);
            setFocusPolicy(Qt::ClickFocus);
            setAutoFillBackground(false);
        }

        virtual ~MainViewMDI()
        {
            if(m_api->parentIsPtr())
                Buffer::Free((buffer) m_api);
        }

        MainViewMDI(const MainViewMDI* rhs) {BLIK_ASSERT("사용금지", false);}
        MainViewMDI& operator=(const MainViewMDI& rhs) {BLIK_ASSERT("사용금지", false); return *this;}

    public:
        void onCloseEvent(QCloseEvent* event)
        {
            closeEvent(event);
            GenericView::CloseAllWindows();
        }

    protected:
        void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE
        {
            m_width = event->size().width();
            m_height = event->size().height();
            m_api->resize(m_width, m_height);
        }

        void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE
        {
            if(m_api->hasViewManager())
                m_api->paint();
            else QMdiArea::paintEvent(event);
            m_api->nextPaint();
        }

    protected:
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE {m_api->closeEvent(event);}
        void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mousePressEvent(event);}
        void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseMoveEvent(event);}
        void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE {m_api->mouseReleaseEvent(event);}
        void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE {m_api->wheelEvent(event);}
        void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyPressEvent(event);}
        void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE {m_api->keyReleaseEvent(event);}
        static void updater(void* data, sint32 count)
        {((MainViewMDI*) data)->m_api->update(count);}

    public:
        ViewAPI* m_api;

    private:
        sint32 m_width;
        sint32 m_height;
    };

    class WidgetBox
    {
    public:
        WidgetBox()
        {
            m_view = nullptr;
            m_widget = nullptr;
        }
        ~WidgetBox()
        {
            delete m_view;
        }

        void setWidget(GenericView* view, QWidget* widget)
        {
            m_view = view;
            m_widget = widget;
        }

    private:
        GenericView* m_view;
        QWidget* m_widget;
    };

    class MainData
    {
    public:
        MainData(QMainWindow* parent)
        {
            m_parent = parent;
            m_viewGL = nullptr;
            m_viewMDI = nullptr;
        }
        ~MainData() {}

    public:
        void onCloseEvent(QCloseEvent* event)
        {
            if(m_viewGL)
                m_viewGL->onCloseEvent(event);
            else if(m_viewMDI)
            {
                m_viewMDI->closeAllSubWindows();
                if(m_viewMDI->currentSubWindow())
                    event->ignore();
                else event->accept();
            }
        }

    public:
        void initForGL(bool frameless)
        {
            m_viewGL = new MainViewGL(m_parent);
            m_viewGL->m_api->renewParent(m_viewGL);
            m_parent->setCentralWidget(m_viewGL);
            if(frameless)
            {
                #if BLIK_MAC_OSX
                    m_parent->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
                #elif BLIK_WINDOWS
                    m_parent->setWindowFlags(Qt::FramelessWindowHint);
                #endif
            }
        }

        void initForMDI(bool frameless)
        {
            m_viewMDI = new MainViewMDI(m_parent);
            m_viewMDI->m_api->renewParent(m_viewMDI);
            m_parent->setCentralWidget(m_viewMDI);
            if(frameless)
            {
                #if BLIK_MAC_OSX
                    m_parent->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
                #elif BLIK_WINDOWS
                    m_parent->setWindowFlags(Qt::FramelessWindowHint);
                #endif
            }
        }

        ViewAPI* getMainAPI()
        {
            BLIK_ASSERT("GL/MDI의 초기화가 되어있지 않습니다", m_viewGL || m_viewMDI);
            if(m_viewGL) return m_viewGL->m_api;
            return m_viewMDI->m_api;
        }

        QGLWidget* getGLWidget()
        {
            return m_viewGL;
        }

        QWidget* addWidget(GenericView* view)
        {
            BLIK_ASSERT("GL/MDI의 초기화가 되어있지 않습니다", m_viewGL || m_viewMDI);
            if(m_viewGL) return view->m_api->getWidget();
            return m_viewMDI->addSubWindow(view->m_api->getWidget());
        }

    public:
        QMenu* getMenu(chars name)
        {
            QMenu** Result = m_menuMap.Access(name);
            if(!Result)
            {
                m_menuMap(name) = g_window->menuBar()->addMenu(QString::fromUtf8(name));
                Result = m_menuMap.Access(name);
            }
            return *Result;
        }

        QToolBar* getToolbar(chars name)
        {
            QToolBar** Result = m_toolbarMap.Access(name);
            if(!Result)
            {
                m_toolbarMap(name) = g_window->addToolBar(QString::fromUtf8(name));
                Result = m_toolbarMap.Access(name);
            }
            return *Result;
        }

    private:
        QMainWindow* m_parent;
        MainViewGL* m_viewGL;
        MainViewMDI* m_viewMDI;
        Map<QMenu*> m_menuMap;
        Map<QToolBar*> m_toolbarMap;
    };

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow()
        {
            setUnifiedTitleAndToolBarOnMac(true);
            g_data = new MainData(this);
        }

        ~MainWindow()
        {
            delete g_data;
            g_data = nullptr;
        }

    protected:
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE
        {
            g_data->onCloseEvent(event);
        }

    private slots:
        void OnSlot()
        {
        }
    };

    class StackMessage
    {
    public:
        static StackMessage& Me()
        {static StackMessage _; return _;}

    public:
        void Push()
        {
            m_strings.AtAdding();
        }

        void Pop()
        {
            BLIK_ASSERT("StackMessage의 Pop이 초과됩니다", 0 < m_strings.Count());
            m_strings.SubtractionOne();
        }

        sint32 Count()
        {
            return m_strings.Count();
        }

        String Get(String str)
        {
            BLIK_ASSERT("StackMessage의 Push가 부족합니다", 0 < m_strings.Count());
            m_strings.At(-1) = str;
            String Result;
            for(sint32 i = 0, iend = m_strings.Count(); i < iend; ++i)
            {
                if(0 < i) Result += " → ";
                Result += m_strings[i];
            }
            return Result;
        }

    private:
        StackMessage()
        {
        }
        ~StackMessage()
        {
        }

        Strings m_strings;
    };

    class Tracker
    {
        typedef void (*ExitCB)(void*);

    public:
        Tracker(QWidget* parent, ExitCB exit, void* data)
        {
            m_parent = parent;
            m_exit = exit;
            m_data = data;
            m_next = nullptr;
            if(m_parent)
                Top().Insert(this);
        }

        ~Tracker()
        {
            if(m_parent)
                Top().Remove(this);
        }

    public:
        void Lock()
        {
            m_loop.exec();
        }

        void Unlock()
        {
            m_loop.quit();
        }

        static void CloseAll(QWidget* parent)
        {
            Tracker* CurNode = Top().m_next;
            while(CurNode)
            {
                Tracker* NextNode = CurNode->m_next;
                if(CurNode->m_parent == parent)
                {
                    Top().Remove(CurNode);
                    CurNode->m_exit(CurNode->m_data);
                }
                CurNode = NextNode;
            }
        }

        static bool HasAnyone(QWidget* parent)
        {
            Tracker* CurNode = Top().m_next;
            while(CurNode)
            {
                Tracker* NextNode = CurNode->m_next;
                if(CurNode->m_parent == parent)
                    return true;
                CurNode = NextNode;
            }
            return false;
        }

    private:
        void Insert(Tracker* rhs)
        {
            rhs->m_next = m_next;
            m_next = rhs;
        }

        void Remove(Tracker* rhs)
        {
            Tracker* CurNode = this;
            while(CurNode)
            {
                if(CurNode->m_next == rhs)
                {
                    CurNode->m_next = CurNode->m_next->m_next;
                    break;
                }
                CurNode = CurNode->m_next;
            }
        }

        static Tracker& Top()
        {static Tracker _(nullptr, nullptr, nullptr); return _;}

    private:
        QWidget* m_parent;
        QEventLoop m_loop;
        ExitCB m_exit;
        void* m_data;
        Tracker* m_next;
    };

    class EditTracker : public QLineEdit
    {
        Q_OBJECT

    public:
        EditTracker(UIEditType type, const QString& contents, QWidget* parent)
            : QLineEdit(contents, nullptr), m_tracker(parent, OnExit, this)
        {
            if(parent)
                m_parentpos = parent->mapToGlobal(QPoint(0, 0));
            else m_parentpos = QPoint(0, 0);

            m_canceled = false;
            setWindowFlags(Qt::SplashScreen);
            setWindowModality(Qt::WindowModal);
            setFocus();
            setFrame(false);
            selectAll();

            switch(type)
            {
            case UIET_Int: setValidator(new QIntValidator(this)); break;
            case UIET_Float: setValidator(new QDoubleValidator(this)); break;
            }

            QPalette Palette = palette();
            Palette.setColor(QPalette::Base, QColor(255, 255, 255));
            setPalette(Palette);
        }

        ~EditTracker()
        {
        }

        bool Popup(sint32 x, sint32 y, sint32 w, sint32 h)
        {
            move(m_parentpos.x() + x, m_parentpos.y() + y);
            resize(w, h);
            show();
            m_tracker.Lock();
            return !m_canceled;
        }

    protected:
        static void OnExit(void* data)
        {
            EditTracker* This = (EditTracker*) data;
            This->m_canceled = true;
            This->close();
            This->m_tracker.Unlock();
        }

    private:
        void focusOutEvent(QFocusEvent* event) Q_DECL_OVERRIDE
        {
            close();
            m_tracker.Unlock();
        }

        void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE
        {
            if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
            {
                close();
                m_tracker.Unlock();
            }
            else if(event->key() == Qt::Key_Escape)
            {
                m_canceled = true;
                close();
                m_tracker.Unlock();
            }
            else QLineEdit::keyPressEvent(event);
        }

    private:
        Tracker m_tracker;
        bool m_canceled;
        QPoint m_parentpos;
    };

    class ListTracker : public QListWidget
    {
        Q_OBJECT

    public:
        ListTracker(const Strings& strings, QWidget* parent)
            : QListWidget(nullptr), m_tracker(parent, OnExit, this)
        {
            for(sint32 i = 0, iend = strings.Count(); i < iend; ++i)
            {
                QListWidgetItem* NewItem = new QListWidgetItem(QString::fromUtf8(strings[i]), this);
                NewItem->setTextAlignment(Qt::AlignCenter);
                addItem(NewItem);
            }

            if(parent)
                m_parentpos = parent->mapToGlobal(QPoint(0, 0));
            else m_parentpos = QPoint(0, 0);

            m_select = -1;
            setWindowFlags(Qt::SplashScreen);
            setWindowModality(Qt::WindowModal);
            setFocus();

            connect(this, SIGNAL(itemPressed(QListWidgetItem*)), SLOT(onItemPressed(QListWidgetItem*)));
        }

        ~ListTracker()
        {
        }

        sint32 Popup(sint32 x, sint32 y, sint32 w, sint32 h)
        {
            move(m_parentpos.x() + x, m_parentpos.y() + y);
            resize(w, h);
            show();
            m_tracker.Lock();
            return m_select;
        }

    protected:
        static void OnExit(void* data)
        {
            ListTracker* This = (ListTracker*) data;
            This->m_select = -1;
            This->close();
            This->m_tracker.Unlock();
        }

    private:
        void focusOutEvent(QFocusEvent* event) Q_DECL_OVERRIDE
        {
            close();
            m_tracker.Unlock();
        }

    private slots:
        void onItemPressed(QListWidgetItem* item)
        {
            m_select = row(item);
            close();
            m_tracker.Unlock();
        }

    private:
        Tracker m_tracker;
        sint32 m_select;
        QPoint m_parentpos;
    };

    class SurfaceClass
    {
    public:
        SurfaceClass() : mFBO(0, 0, QOpenGLFramebufferObjectFormat()), mDevice(0, 0)
        {
            BLIK_ASSERT("잘못된 시나리오입니다", false);
        }
        SurfaceClass(sint32 width, sint32 height, QOpenGLFramebufferObjectFormat* format)
            : mFBO(width, height, *format), mDevice(width, height)
        {
        }
        ~SurfaceClass()
        {
            mFBO.release();
        }

    public:
        SurfaceClass(const SurfaceClass& rhs) : mFBO(0, 0, QOpenGLFramebufferObjectFormat()), mDevice(0, 0)
        {
            operator=(rhs);
        }
        SurfaceClass& operator=(const SurfaceClass& rhs)
        {
            BLIK_ASSERT("잘못된 시나리오입니다", false);
            return *this;
        }

    public:
        inline sint32 width() const {return mFBO.width();}
        inline sint32 height() const {return mFBO.height();}
        inline QPainter* painter() {return &mCanvas.painter();}

    public:
        void BindGraphics()
        {
            mCanvas.Bind(&mDevice);
            mFBO.bind();
        }
        void UnbindGraphics()
        {
            mLastImage = mFBO.toImage();
            mCanvas.Unbind();
        }

    private:
        QOpenGLFramebufferObject mFBO;
        QOpenGLPaintDevice mDevice;
        CanvasClass mCanvas;

    public:
        QImage mLastImage;
    };

    class ThreadClass : public QThread
    {
        Q_OBJECT

    public:
        static void Begin(ThreadCB cb, payload data)
        {
            new ThreadClass(cb, data);
        }

    private:
        ThreadClass(ThreadCB cb, payload data)
        {
            BLIK_ASSERT("cb이 nullptr이 될 수 없습니다", cb);
            m_cb = cb;
            m_data = data;
            start();
        }
        virtual ~ThreadClass() {}

    private:
        void run()
        {
            m_cb(m_data);
            exec();
        }

    private:
        ThreadCB m_cb;
        payload m_data;
    };

    class Clock
    {
    public:
        Clock()
        {
            m_prev = GetHead();
            m_next = m_prev->m_next;
            m_prev->m_next = this;
            m_next->m_prev = this;
            if(!GetTimer().isValid())
            {
                const sint64 LocalTimeMSecFromUtc = (sint64) (QDateTime::currentDateTime().utcOffset() * 1000);
                GetTotalMSecFromJulianDay() = ((sint64) EpochToJulian(QDateTime::currentMSecsSinceEpoch())) + LocalTimeMSecFromUtc;
                GetTimer().start();
            }
            m_laptime = GetTimer().nsecsElapsed();
        }
        Clock(const Clock& rhs)
        {
            m_prev = GetHead();
            m_next = m_prev->m_next;
            m_prev->m_next = this;
            m_next->m_prev = this;
            operator=(rhs);
        }
        ~Clock()
        {
            m_prev->m_next = m_next;
            m_next->m_prev = m_prev;
        }

    public:
        void SetClock(sint32 year, sint32 month, sint32 day, sint32 hour, sint32 min, sint32 sec, sint64 nsec)
        {
            QDateTime NewTime = QDateTime::fromString(
                (chars) String::Format("%04d-%02d-%02d %02d:%02d:%02d",
                year, month, day, hour, min, sec), "yyyy-MM-dd HH:mm:ss");
            const sint64 LocalTimeMSecFromUtc = (sint64) (QDateTime::currentDateTime().utcOffset() * 1000);
            const sint64 NewClockMSec = ((sint64) EpochToJulian(NewTime.toMSecsSinceEpoch())) + LocalTimeMSecFromUtc;
            m_laptime = (NewClockMSec - GetTotalMSecFromJulianDay()) * 1000000 + nsec;
        }

    public:
        inline Clock& operator=(const Clock& rhs)
        {m_laptime = rhs.m_laptime; return *this;}

    public:
        inline sint64 GetLap() const {return m_laptime;}
        inline void AddLap(sint64 nsec) {m_laptime += nsec;}
        inline sint64 GetTotalSec() const
        {return GetTotalMSecFromJulianDay() / 1000 + m_laptime / 1000000000;}
        inline sint64 GetNSecInSec() const
        {return ((GetTotalMSecFromJulianDay() % 1000) * 1000000 + 1000000000 + (m_laptime % 1000000000)) % 1000000000;}

    public:
        static void SetBaseTime(chars timestring)
        {
            QDateTime CurrentTime = QDateTime::fromString(timestring, "yyyy-MM-dd HH:mm:ss");
            const sint64 LocalTimeMSecFromUtc = (sint64) (QDateTime::currentDateTime().utcOffset() * 1000);
            GetTotalMSecFromJulianDay() = ((sint64) EpochToJulian(CurrentTime.toMSecsSinceEpoch())) + LocalTimeMSecFromUtc;

            const sint64 ChangedNSec = GetTimer().restart() * 1000000;
            Clock* CurNode = GetHead();
            while((CurNode = CurNode->m_next) != GetHead())
                CurNode->m_laptime -= ChangedNSec;
        }

    private:
        static Clock* GetHead() {static Clock _(0); return &_;}
        static QElapsedTimer& GetTimer() {static QElapsedTimer _; return _;}
        static sint64& GetTotalMSecFromJulianDay() {static sint64 _ = 0; return _;}

    private:
        Clock(int) // Head전용
        {
            m_prev = this;
            m_next = this;
        }

    private:
        Clock* m_prev;
        Clock* m_next;
        sint64 m_laptime;
    };

    class TCPPeerData : public QObjectUserData
    {
    public:
        const sint32 ID;
        sint32 PacketNeeds;

    public:
        TCPPeerData() : ID(MakeID()) {PacketNeeds = 0;}
        virtual ~TCPPeerData() {}

    public:
        static uint ClassID()
        {static uint _ = QObject::registerUserData(); return _;}
        static int MakeID() {static int _ = -1; return ++_;}
    };

    class TCPPacket
    {
    public:
        const packettype Type;
        const sint32 PeerID;
        buffer Buffer;

    public:
        TCPPacket(packettype type, sint32 peerid, sint32 buffersize)
            : Type(type), PeerID(peerid) {Buffer = Buffer::Alloc(BLIK_DBG buffersize);}
        ~TCPPacket() {Buffer::Free(Buffer);}
    public:
        void DestroyMe() {delete this;}
    };

    class TCPAgent : public QTcpServer
    {
        Q_OBJECT

    private:
        bool UsingSizeField;
        Map<QTcpSocket*> Peers;
        Queue<TCPPacket*> PacketQueue;
        TCPPacket* FocusedPacket;

    private slots:
        void acceptPeer()
        {
            QTcpSocket* Peer = nextPendingConnection();
            TCPPeerData* NewData = new TCPPeerData();
            Peers[NewData->ID] = Peer;
            Peer->setUserData(TCPPeerData::ClassID(), NewData);
            PacketQueue.Enqueue(new TCPPacket(packettype_entrance, NewData->ID, 0));

            if(!UsingSizeField) connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeer()));
            else connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeerWithSizeField()));
            connect(Peer, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(errorPeer(QAbstractSocket::SocketError)));
        }

        void readyPeer()
        {
            QTcpSocket* Peer = (QTcpSocket*) sender();
            TCPPeerData* Data = (TCPPeerData*) Peer->userData(TCPPeerData::ClassID());
            sint64 PacketSize = Peer->bytesAvailable();

            TCPPacket* NewPacket = new TCPPacket(packettype_message, Data->ID, PacketSize);
            Peer->read((char*) NewPacket->Buffer, PacketSize);
            PacketQueue.Enqueue(NewPacket);

            connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeer()));
        }

        void readyPeerWithSizeField()
        {
            QTcpSocket* Peer = (QTcpSocket*) sender();
            TCPPeerData* Data = (TCPPeerData*) Peer->userData(TCPPeerData::ClassID());
            sint64 PacketSize = Peer->bytesAvailable();

            while(0 < PacketSize)
            {
                if(Data->PacketNeeds == 0)
                {
                    if(4 <= PacketSize)
                    {
                        PacketSize -= 4;
                        int GetPacketSize = 0;
                        Peer->read((char*) &GetPacketSize, 4);
                        Data->PacketNeeds = GetPacketSize;
                    }
                    else
                    {
                        connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeerWithSizeField()));
                        break;
                    }
                }
                if(0 < Data->PacketNeeds)
                {
                    if(Data->PacketNeeds <= PacketSize)
                    {
                        PacketSize -= Data->PacketNeeds;
                        TCPPacket* NewPacket = new TCPPacket(packettype_message, Data->ID, Data->PacketNeeds);
                        Peer->read((char*) NewPacket->Buffer, Data->PacketNeeds);
                        PacketQueue.Enqueue(NewPacket);
                        Data->PacketNeeds = 0;
                    }
                    else
                    {
                        connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeerWithSizeField()));
                        break;
                    }
                }
            }
        }

        void errorPeer(QAbstractSocket::SocketError error)
        {
            QTcpSocket* Peer = (QTcpSocket*) sender();
            TCPPeerData* Data = (TCPPeerData*) Peer->userData(TCPPeerData::ClassID());
            Peers.Remove(Data->ID);
            PacketQueue.Enqueue(new TCPPacket((error == QAbstractSocket::RemoteHostClosedError)?
                packettype_leaved : packettype_kicked, Data->ID, 0));
        }

    public:
        TCPAgent(bool sizefield = false)
        {
            UsingSizeField = sizefield;
            FocusedPacket = new TCPPacket(packettype_null, -1, 0);
            connect(this, SIGNAL(newConnection()), this, SLOT(acceptPeer()));
        }

        virtual ~TCPAgent()
        {
            for(TCPPacket* UnusedPacket = nullptr; UnusedPacket = PacketQueue.Dequeue();)
                UnusedPacket->DestroyMe();
            delete FocusedPacket;
        }

        TCPAgent& operator=(const TCPAgent&)
        {
            BLIK_ASSERT("호출불가", false);
            return *this;
        }

    public:
        bool TryPacket()
        {
            TCPPacket* PopPacket = PacketQueue.Dequeue();
            if(!PopPacket) return false;
            delete FocusedPacket;
            FocusedPacket = PopPacket;
            return true;
        }

        TCPPacket* GetFocusedPacket()
        {
            return FocusedPacket;
        }

        bool SendPacket(int peerid, const void* buffer, sint32 buffersize)
        {
            QTcpSocket* Peer = Peers[peerid];
            if(!Peer)
            {
                Peers.Remove(peerid);
                return false;
            }
            return (Peer->write((chars) buffer, buffersize) == buffersize);
        }
    };

    class WebViewPrivate : public QWebEngineView
    {
        Q_OBJECT

    public:
        WebViewPrivate(QWidget* parent = nullptr) : QWebEngineView(parent), mHandle(h_web::null())
        {
            mCb = nullptr;
            mData = nullptr;
            setMouseTracking(true);
            connect(this, SIGNAL(urlChanged(QUrl)), this, SLOT(urlEvent(QUrl)));
        }
        virtual ~WebViewPrivate()
        {
            mHandle.set_buf(nullptr);
        }

    public:
        WebViewPrivate(const WebViewPrivate&) {BLIK_ASSERT("사용금지", false);}
        WebViewPrivate& operator=(const WebViewPrivate&) {BLIK_ASSERT("사용금지", false); return *this;}

    protected:
        void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE
        {
            event->accept();
            mHandle.set_buf(nullptr);
        }

    private slots:
        virtual void urlEvent(const QUrl& url)
        {
            if(mCb)
                mCb(mData, "UrlChanged", url.url().toUtf8().constData());
        }

    public:
        h_web mHandle;
        Platform::Web::EventCB mCb;
        payload mData;
    };

    class WebPrivate
    {
    public:
        WebPrivate()
        {
            mProxy = mScene.addWidget(&mView);
        }
        ~WebPrivate()
        {
            mScene.removeItem(mProxy);
        }

    public:
        WebPrivate(const WebPrivate&) {BLIK_ASSERT("사용금지", false);}
        WebPrivate& operator=(const WebPrivate&) {BLIK_ASSERT("사용금지", false); return *this;}

    public:
        void attachHandle(h_web web)
        {
            mView.mHandle = web;
        }
        void Reload(chars url)
        {
            mView.load(QUrl(QString(url)));
        }
        void Resize(sint32 width, sint32 height)
        {
            if(width != mLastImage.width() || height != mLastImage.height())
            {
                mView.resize(width, height);
                mLastImage = QImage(width, height, QImage::Format_ARGB32);
            }
        }
        void SetCallback(Platform::Web::EventCB cb, payload data)
        {
            mView.mCb = cb;
            mView.mData = data;
        }
        void SendTouchEvent(TouchType type, sint32 x, sint32 y)
        {
            QMouseEvent::Type CurType = QMouseEvent::None;
            switch(type)
            {
            case TT_Moving: CurType = QMouseEvent::MouseMove; break;
            case TT_Press: CurType = QMouseEvent::MouseButtonPress; break;
            case TT_Dragging: CurType = QMouseEvent::MouseMove; break;
            case TT_Release: CurType = QMouseEvent::MouseButtonRelease; break;
            default: BLIK_ASSERT("해당 case가 준비되지 않았습니다", false);
            }
            QMouseEvent NewEvent(CurType, QPoint(x, y), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            Q_FOREACH(QObject* obj, mView.page()->view()->children())
            {
                if(qobject_cast<QWidget*>(obj))
                    QApplication::sendEvent(obj, &NewEvent);
            }
        }
        void SendKeyEvent(sint32 code, chars text, bool pressed)
        {
            if(auto CurWidget = mView.focusProxy())
            {
                QKeyEvent NewEvent((pressed)? QKeyEvent::KeyPress : QKeyEvent::KeyRelease, code, Qt::NoModifier, text);
                QApplication::sendEvent(CurWidget, &NewEvent);
            }
        }
        const QPixmap GetPixmap()
        {
            return QPixmap::fromImage(GetImage());
        }
        const QImage& GetImage()
        {
            mView.update();
            CanvasClass CurCanvas(&mLastImage);
            const QRect CurRect(0, 0, mLastImage.width(), mLastImage.height());
            mScene.render(&CurCanvas.painter(), CurRect, CurRect);
            return mLastImage;
        }

    private:
        WebViewPrivate mView;
        QGraphicsProxyWidget* mProxy;
        QGraphicsScene mScene;
        QImage mLastImage;
    };

    #if BLIK_ANDROID
        class SerialPortForAndroid
        {
        public:
            SerialPortForAndroid(const QSerialPortInfo& info)
            {
                mPortName = info.portName().toUtf8().constData();
                mLocation = info.systemLocation().toUtf8().constData();
                mBaudRate = QSerialPort::UnknownBaud;
                mDataBits = QSerialPort::UnknownDataBits;
                mParity = QSerialPort::UnknownParity;
                mStopBits = QSerialPort::UnknownStopBits;
                mFlowControl = QSerialPort::UnknownFlowControl;
                mFd = -1;
            }
            ~SerialPortForAndroid()
            {
                close();
            }

        public:
            bool open(QIODevice::OpenModeFlag flag)
            {
                close();
                int mode = 0;
                switch(flag)
                {
                case QIODevice::ReadOnly: mode = O_RDONLY | O_NOCTTY | O_NONBLOCK; break;
                case QIODevice::WriteOnly: mode = O_WRONLY | O_NOCTTY | O_NONBLOCK; break;
                case QIODevice::ReadWrite: mode = O_RDWR | O_NOCTTY | O_NONBLOCK; break;
                default: BLIK_ASSERT("알 수 없는 flag값입니다", false); return false;
                }
                const int fd = ::open(mLocation, mode);
                BLIK_TRACE("SerialPortForAndroid::open(%s, %d) fd = %d", (chars) mLocation, mode, fd);
                if(fd == -1)
                {
                    if(errno == EINTR) // Recurse because this is a recoverable error
                        return open(flag);
                    return false;
                }
                mFd = reconfigurePort(fd);
                return true;
            }
            void close()
            {
                if(mFd != -1)
                {
                    ::close(mFd);
                    mFd = -1;
                }
            }
            bool waitForReadyRead(int msecs)
            {
                if(mFd != -1)
                {
                    fd_set readfds;
                    FD_ZERO(&readfds);
                    FD_SET(mFd, &readfds);
                    timespec timeout_ts(timespec_from_ms(msecs));
                    if(0 < pselect(mFd + 1, &readfds, nullptr, nullptr, &timeout_ts, nullptr))
                        return (FD_ISSET(mFd, &readfds) != 0);
                }
                return false;
            }
            QByteArray readAll()
            {
                QByteArray Result;
                if(mFd != -1)
                {
                    char buf[1024];
                    ssize_t bytes_read = ::read(mFd, buf, sizeof(buf));
                    if(0 < bytes_read)
                        Result.append(buf, bytes_read);
                }
                return Result;
            }
            qint64 write(const char* data, qint64 len)
            {
                if(mFd == -1) return 0;
                size_t data_focus = 0;
                while(data_focus < len)
                {
                    fd_set writefds;
                    FD_ZERO(&writefds);
                    FD_SET(mFd, &writefds);
                    timespec timeout_ts(timespec_from_ms(10));
                    int ret = pselect(mFd + 1, nullptr, &writefds, nullptr, &timeout_ts, nullptr);
                    if(ret < 0)
                    {
                        if(errno == EINTR) continue;
                        return false;
                    }
                    else if(0 < ret)
                    {
                        if(FD_ISSET(mFd, &writefds) != 0)
                        {
                            const ssize_t bytes_written_now = ::write(mFd, data + data_focus, len - data_focus);
                            if(bytes_written_now < 1) break;
                            data_focus += bytes_written_now;
                        }
                    }
                }
                return data_focus;
            }
            bool flush()
            {
                return (mFd != -1);
            }

        public: // setter
            void setBaudRate(QSerialPort::BaudRate baudrate) {mBaudRate = baudrate;}
            void setDataBits(QSerialPort::DataBits databits) {mDataBits = databits;}
            void setParity(QSerialPort::Parity parity) {mParity = parity;}
            void setStopBits(QSerialPort::StopBits stopbits) {mStopBits = stopbits;}
            void setFlowControl(QSerialPort::FlowControl flowcontrol) {mFlowControl = flowcontrol;}

        public: // getter
            QString portName() {return (chars) mPortName;}
            QString errorString() {return "NoError";}
            QSerialPort::SerialPortError error() {return QSerialPort::NoError;}

        private:
            String mPortName;
            String mLocation;
            QSerialPort::BaudRate mBaudRate;
            QSerialPort::DataBits mDataBits;
            QSerialPort::Parity mParity;
            QSerialPort::StopBits mStopBits;
            QSerialPort::FlowControl mFlowControl;
            int mFd;

        private:
            int reconfigurePort(const int fd)
            {
                struct termios options;
                if(tcgetattr(fd, &options) == -1)
                {
                    ::close(fd);
                    return -1;
                }

                // set up raw mode / no echo / binary
                options.c_cflag |= (tcflag_t)  (CLOCAL | CREAD);
                options.c_lflag &= (tcflag_t) ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN);
                options.c_oflag &= (tcflag_t) ~(OPOST);
                options.c_iflag &= (tcflag_t) ~(INLCR | IGNCR | ICRNL | IGNBRK);
                #ifdef IUCLC
                    options.c_iflag &= (tcflag_t) ~IUCLC;
                #endif
                #ifdef PARMRK
                    options.c_iflag &= (tcflag_t) ~PARMRK;
                #endif

                // setup baud rate
                speed_t baud;
                switch(mBaudRate)
                {
                case QSerialPort::Baud1200: baud = B1200; break;
                case QSerialPort::Baud2400: baud = B2400; break;
                case QSerialPort::Baud4800: baud = B4800; break;
                default: BLIK_ASSERT("알 수 없는 mBaudRate값입니다", false);
                case QSerialPort::Baud9600: baud = B9600; break;
                case QSerialPort::Baud19200: baud = B19200; break;
                case QSerialPort::Baud38400: baud = B38400; break;
                case QSerialPort::Baud57600: baud = B57600; break;
                case QSerialPort::Baud115200: baud = B115200; break;
                }
                #ifdef _BSD_SOURCE
                    ::cfsetspeed(&options, baud);
                #else
                    ::cfsetispeed(&options, baud);
                    ::cfsetospeed(&options, baud);
                #endif

                // setup char len
                options.c_cflag &= (tcflag_t) ~CSIZE;
                switch(mDataBits)
                {
                case QSerialPort::Data5: options.c_cflag |= CS5; break;
                case QSerialPort::Data6: options.c_cflag |= CS6; break;
                case QSerialPort::Data7: options.c_cflag |= CS7; break;
                default: BLIK_ASSERT("알 수 없는 mDataBits값입니다", false);
                case QSerialPort::Data8: options.c_cflag |= CS8; break;
                }

                // setup stopbits
                switch(mStopBits)
                {
                default: BLIK_ASSERT("알 수 없는 mStopBits값입니다", false);
                case QSerialPort::OneStop: options.c_cflag &= (tcflag_t) ~(CSTOPB); break;
                case QSerialPort::OneAndHalfStop: options.c_cflag |= (CSTOPB); break;
                case QSerialPort::TwoStop: options.c_cflag |= (CSTOPB); break;
                }

                // setup parity
                options.c_iflag &= (tcflag_t) ~(INPCK | ISTRIP);
                switch(mParity)
                {
                default: BLIK_ASSERT("알 수 없는 mParity값입니다", false);
                case QSerialPort::NoParity: options.c_cflag &= (tcflag_t) ~(PARENB | PARODD); break;
                case QSerialPort::EvenParity: options.c_cflag &= (tcflag_t) ~(PARODD); options.c_cflag |= (PARENB); break;
                case QSerialPort::OddParity: options.c_cflag |= (PARENB | PARODD); break;
                case QSerialPort::SpaceParity: options.c_cflag |= (PARENB | CMSPAR); options.c_cflag &= (tcflag_t) ~(PARODD); break;
                case QSerialPort::MarkParity: options.c_cflag |= (PARENB | CMSPAR | PARODD); break;
                }
                #ifndef CMSPAR
                    if(mParity == QSerialPort::SpaceParity || mParity == QSerialPort::MarkParity)
                        BLIK_ASSERT("OS에서 지원하지 않는 mParity값입니다", false);
                #endif

                // setup flow control
                bool xonxoff, rtscts;
                switch(mFlowControl)
                {
                default: BLIK_ASSERT("알 수 없는 mFlowControl값입니다", false);
                case QSerialPort::NoFlowControl: xonxoff = false; rtscts = false; break;
                case QSerialPort::HardwareControl: xonxoff = false; rtscts = true; break;
                case QSerialPort::SoftwareControl: xonxoff = true; rtscts = false; break;
                }
                // xonxoff
                #ifdef IXANY
                    if(xonxoff) options.c_iflag |= (IXON | IXOFF);
                    else options.c_iflag &= (tcflag_t) ~(IXON | IXOFF | IXANY);
                #else
                    if(xonxoff) options.c_iflag |= (IXON | IXOFF);
                    else options.c_iflag &= (tcflag_t) ~(IXON | IXOFF);
                #endif
                // rtscts
                #ifdef CRTSCTS
                    if(rtscts) options.c_cflag |= (CRTSCTS);
                    else options.c_cflag &= (unsigned long) ~(CRTSCTS);
                #elif defined CNEW_RTSCTS
                    if(rtscts) options.c_cflag |= (CNEW_RTSCTS);
                    else options.c_cflag &= (unsigned long) ~(CNEW_RTSCTS);
                #else
                    BLIK_ASSERT("OS에서 지원하지 않는 rtscts값입니다", false);
                #endif

                options.c_cc[VMIN] = 0;
                options.c_cc[VTIME] = 0;
                if(tcsetattr(fd, TCSANOW, &options) == -1)
                {
                    ::close(fd);
                    return -1;
                }
                return fd;
            }
            timespec timespec_from_ms(const uint32_t millis)
            {
                timespec time;
                time.tv_sec = millis / 1e3;
                time.tv_nsec = (millis - (time.tv_sec * 1e3)) * 1e6;
                return time;
            }
        };
        typedef SerialPortForAndroid SerialPortClass;
    #else
        typedef QSerialPort SerialPortClass;
    #endif

    class SerialClass
    {
    private:
        SerialPortClass* mSerial;
        SerialDecodeCB mDec;
        SerialEncodeCB mEnc;
        uint08s mReadStream;
        uint08s mWriteStream;
        uint08s mLastDecodedData;
        sint32 mLastDecodedDataFocus;

    private:
        class Chunk
        {
        public:
            uint08* mPtr;
            bytes mPtrConst;
            sint32 mLength;
            bool mBigEndian;
        public:
            Chunk() {mPtr = nullptr; mPtrConst = nullptr; mLength = 0; mBigEndian = false;}
            Chunk(uint08* ptr, sint32 length, bool bigendian) {Init(ptr, length, bigendian);}
            Chunk(bytes ptr, sint32 length, bool bigendian) {InitConst(ptr, length, bigendian);}
            ~Chunk() {}
            void Init(uint08* ptr, sint32 length, bool bigendian)
            {
                mPtr = ptr;
                mPtrConst = nullptr;
                mLength = length;
                mBigEndian = bigendian;
            }
            void InitConst(bytes ptr, sint32 length, bool bigendian)
            {
                mPtr = nullptr;
                mPtrConst = ptr;
                mLength = length;
                mBigEndian = bigendian;
            }
            sint64 GetLength() const
            {
                sint64 Result;
                Load(&Result, sizeof(sint64));
                return Result;
            }
            void Load(void* outptr, sint32 length) const
            {
                Memory::Set(outptr, 0, length);
                const sint32 CopyLength = Math::Min(mLength, length);
                if(!mBigEndian) Memory::Copy(outptr, mPtrConst, CopyLength);
                else
                {
                    uint08* Dest = ((uint08*) outptr) + CopyLength - 1;
                    for(sint32 i = 0; i < CopyLength; ++i)
                        Dest[-i] = mPtrConst[i];
                }
            }
            void Save(bytes ptr, sint32 length)
            {
                Memory::Set(mPtr, 0, mLength);
                const sint32 CopyLength = Math::Min(mLength, length);
                if(!mBigEndian) Memory::Copy(mPtr, ptr, CopyLength);
                else
                {
                    uint08* Dest = ((uint08*) mPtr) + CopyLength - 1;
                    for(sint32 i = 0; i < CopyLength; ++i)
                        Dest[-i] = ptr[i];
                }
            }
        };

    public:
        static Strings GetList(String* spec)
        {
            Strings Result;
            Context SpecCollector;
            const QList<QSerialPortInfo>& AllPorts = QSerialPortInfo::availablePorts();
            foreach(const auto& CurPort, AllPorts)
            {
                String CurName = CurPort.portName().toUtf8().constData();
                if(CurName.Length() == 0)
                    CurName = CurPort.description().toUtf8().constData();
                Result.AtAdding() = CurName;
                if(spec)
                {
                    Context& NewChild = SpecCollector.At(SpecCollector.LengthOfIndexable());
                    NewChild.At("portname").Set(CurPort.portName().toUtf8().constData());
                    NewChild.At("description").Set(CurPort.description().toUtf8().constData());
                    NewChild.At("systemlocation").Set(CurPort.systemLocation().toUtf8().constData());
                    NewChild.At("manufacturer").Set(CurPort.manufacturer().toUtf8().constData());
                    NewChild.At("serialnumber").Set(CurPort.serialNumber().toUtf8().constData());
                }
            }
            if(spec)
                *spec = SpecCollector.SaveJson(*spec);
            return Result;
        }

    public:
        SerialClass(chars name, SerialDecodeCB dec, SerialEncodeCB enc)
        {
            mSerial = nullptr;
            mDec = (dec)? dec : DefaultSerialDecode;
            mEnc = (enc)? enc : DefaultSerialEncode;
            mLastDecodedDataFocus = 0;
            const QList<QSerialPortInfo>& AllPorts = QSerialPortInfo::availablePorts();
            foreach(const auto& CurPort, AllPorts)
            {
                if(*name == '\0' || CurPort.portName() == name || CurPort.description() == name)
                {
                    mSerial = new SerialPortClass(CurPort);
                    mSerial->setBaudRate(QSerialPort::Baud115200);
                    mSerial->setDataBits(QSerialPort::Data8);
                    mSerial->setParity(QSerialPort::NoParity);
                    mSerial->setStopBits(QSerialPort::OneStop);
                    mSerial->setFlowControl(QSerialPort::NoFlowControl);
                    if(!mSerial->open(QIODevice::ReadWrite))
                    {
                        BLIK_TRACE("QSerialPort error: port=%s, error=%s(%d)",
                            mSerial->portName().toUtf8().constData(),
                            mSerial->errorString().toUtf8().constData(),
                            (sint32) mSerial->error());
                        delete mSerial;
                        mSerial = nullptr;
                        if(*name == '\0') continue;
                    }
                    break;
                }
            }
        }

        ~SerialClass()
        {
            delete mSerial;
        }

    public:
        bool IsValid()
        {
            return (mSerial != nullptr);
        }

        bool Connected()
        {
            return (mSerial->error() == QSerialPort::NoError);
        }

        bool ReadReady(sint32* gettype)
        {
            // 데이터수신시 읽기스트림에 추가연결
            if(mSerial->waitForReadyRead(1))
            {
                QByteArray NewArray = mSerial->readAll();
                Memory::Copy(mReadStream.AtDumpingAdded(NewArray.length()), NewArray.constData(), NewArray.length());
            }

            // 읽기스트림의 처리
            if(0 < mReadStream.Count())
            {
                // 디코딩과정
                mLastDecodedData.SubtractionAll();
                mLastDecodedDataFocus = 0;
                const sint32 UsedLength = mDec(&mReadStream[0], mReadStream.Count(), mLastDecodedData, gettype);
                if(0 < UsedLength)
                {
                    BLIK_ASSERT("SerialDecodeCB의 리턴값이 허용범위를 초과합니다", UsedLength <= mReadStream.Count());
                    const sint32 CopyLength = mReadStream.Count() - UsedLength;
                    if(0 < CopyLength)
                    {
                        uint08s NewReadStream;
                        Memory::Copy(NewReadStream.AtDumping(0, CopyLength), &mReadStream[UsedLength], CopyLength);
                        mReadStream = NewReadStream;
                    }
                    else mReadStream.SubtractionAll();
                    return (0 < mLastDecodedData.Count());
                }
            }
            return false;
        }

        sint32 ReadAvailable()
        {
            return mLastDecodedData.Count() - mLastDecodedDataFocus;
        }

        sint32 Read(chars format, va_list args)
        {
            Map<Chunk> Cache;
            chars Pos = format;
            bool CurEndian = false;
            sint32 CurIndex = -1;
            while(*Pos)
            {
                if(*Pos == '#')
                {
                    CurIndex++;
                    // 인용길이
                    sint64 LastLength = 0;
                         if('1' <= Pos[1] && Pos[1] <= '8') LastLength = Pos[1] - '0';
                    else if('a' <= Pos[1] && Pos[1] <= 'z') LastLength = Cache[Pos[1] - 'a'].GetLength();
                    else if('A' <= Pos[1] && Pos[1] <= 'Z') LastLength = Cache[Pos[1] - 'A'].GetLength();
                    else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                    if(ReadAvailable() < LastLength) return BLIK_SERIAL_ERROR_SHORT_STREAM - CurIndex;
                    Pos += 2;

                    bytes LastPtr = &mLastDecodedData[mLastDecodedDataFocus];
                    // 캐시로 읽기
                    if(*Pos == '=')
                    {
                             if('a' <= Pos[1] && Pos[1] <= 'z') Cache[Pos[1] - 'a'].InitConst(LastPtr, LastLength, CurEndian);
                        else if('A' <= Pos[1] && Pos[1] <= 'Z') Cache[Pos[1] - 'A'].InitConst(LastPtr, LastLength, CurEndian);
                        else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                        Pos += 2;
                    }
                    // 타입힌트식 읽기
                    else if(*Pos == ':')
                    {
                        if(Pos[1] == 's' && Pos[2] == 'k' && Pos[3] == 'i' && Pos[4] == 'p') Pos += 5;
                        else
                        {
                            const Chunk NewChunk(LastPtr, LastLength, CurEndian);
                            if(Pos[1] == 's')
                            {
                                     if(Pos[2] == '1') NewChunk.Load(va_arg(args, sint08*), sizeof(sint08));
                                else if(Pos[2] == '2') NewChunk.Load(va_arg(args, sint16*), sizeof(sint16));
                                else if(Pos[2] == '4') NewChunk.Load(va_arg(args, sint32*), sizeof(sint32));
                                else if(Pos[2] == '8') NewChunk.Load(va_arg(args, sint64*), sizeof(sint64));
                                else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                            }
                            else if(Pos[1] == 'u')
                            {
                                     if(Pos[2] == '1') NewChunk.Load(va_arg(args, uint08*), sizeof(uint08));
                                else if(Pos[2] == '2') NewChunk.Load(va_arg(args, uint16*), sizeof(uint16));
                                else if(Pos[2] == '4') NewChunk.Load(va_arg(args, uint32*), sizeof(uint32));
                                else if(Pos[2] == '8') NewChunk.Load(va_arg(args, uint64*), sizeof(uint64));
                                else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                            }
                            else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                            Pos += 3;
                        }
                    }
                    // 일반적인 읽기
                    else Chunk(LastPtr, LastLength, CurEndian).Load(va_arg(args, void*), LastLength);
                    mLastDecodedDataFocus += LastLength;
                }
                else if(*Pos == '[')
                {
                         if(Pos[1] == 'b' && Pos[2] == 'e' && Pos[3] == ']') CurEndian = true;
                    else if(Pos[1] == 'l' && Pos[2] == 'e' && Pos[3] == ']') CurEndian = false;
                    else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                    Pos += 4;
                }
                else Pos++;
            }
            return ReadAvailable();
        }

        sint32 Write(chars format, va_list args)
        {
            #if BLIK_ANDROID
                #define va_sint08 sint32
                #define va_sint16 sint32
                #define va_sint32 sint32
                #define va_sint64 sint64
                #define va_uint08 uint32
                #define va_uint16 uint32
                #define va_uint32 uint32
                #define va_uint64 uint64
            #else
                #define va_sint08 sint08
                #define va_sint16 sint16
                #define va_sint32 sint32
                #define va_sint64 sint64
                #define va_uint08 uint08
                #define va_uint16 uint16
                #define va_uint32 uint32
                #define va_uint64 uint64
            #endif

            chars Pos = format;
            bool CurEndian = false;
            while(*Pos)
            {
                if(*Pos == '#')
                {
                    // 인용길이
                    sint64 LastLength = 0;
                    if('1' <= Pos[1] && Pos[1] <= '8') LastLength = Pos[1] - '0';
                    else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                    Pos += 2;

                    uint08* LastPtr = mWriteStream.AtDumpingAdded(LastLength);
                    // 상수로 쓰기
                    if(*Pos == '=')
                    {
                        const sint32 SavedPos = &Pos[-1] - format;
                        uint08s Value;
                        if(Pos[1] == '0' && (Pos[2] == 'x' || Pos[2] == 'X'))
                        {
                            Pos += 2;
                            bool AddingTurn = false;
                            while(*(++Pos))
                            {
                                if(AddingTurn = !AddingTurn)
                                {
                                         if('0' <= *Pos && *Pos <= '9') Value.AtAdding() = ((*Pos - '0') & 0xF) << 4;
                                    else if('a' <= *Pos && *Pos <= 'f') Value.AtAdding() = ((*Pos - 'a' + 10) & 0xF) << 4;
                                    else if('A' <= *Pos && *Pos <= 'F') Value.AtAdding() = ((*Pos - 'A' + 10) & 0xF) << 4;
                                    else break;
                                }
                                else
                                {
                                         if('0' <= *Pos && *Pos <= '9') Value.At(-1) |= (*Pos - '0') & 0xF;
                                    else if('a' <= *Pos && *Pos <= 'f') Value.At(-1) |= (*Pos - 'a' + 10) & 0xF;
                                    else if('A' <= *Pos && *Pos <= 'F') Value.At(-1) |= (*Pos - 'A' + 10) & 0xF;
                                    else break;
                                }
                            }
                        }
                        else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                        // 16진수는 큰수가 메모리의 앞쪽에 오므로 CurEndian을 반대로 적용
                        Chunk(LastPtr, LastLength, !CurEndian).Save(&Value[0], Value.Count());
                    }
                    // 타입힌트식 쓰기
                    else if(*Pos == ':')
                    {
                        Chunk NewChunk(LastPtr, LastLength, CurEndian);
                        if(Pos[1] == 's')
                        {
                                 if(Pos[2] == '1') {auto v = va_arg(args, va_sint08); NewChunk.Save((bytes) &v, sizeof(v));}
                            else if(Pos[2] == '2') {auto v = va_arg(args, va_sint16); NewChunk.Save((bytes) &v, sizeof(v));}
                            else if(Pos[2] == '4') {auto v = va_arg(args, va_sint32); NewChunk.Save((bytes) &v, sizeof(v));}
                            else if(Pos[2] == '8') {auto v = va_arg(args, va_sint64); NewChunk.Save((bytes) &v, sizeof(v));}
                            else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                        }
                        else if(Pos[1] == 'u')
                        {
                                 if(Pos[2] == '1') {auto v = va_arg(args, va_uint08); NewChunk.Save((bytes) &v, sizeof(v));}
                            else if(Pos[2] == '2') {auto v = va_arg(args, va_uint16); NewChunk.Save((bytes) &v, sizeof(v));}
                            else if(Pos[2] == '4') {auto v = va_arg(args, va_uint32); NewChunk.Save((bytes) &v, sizeof(v));}
                            else if(Pos[2] == '8') {auto v = va_arg(args, va_uint64); NewChunk.Save((bytes) &v, sizeof(v));}
                            else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                        }
                        else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                        Pos += 3;
                    }
                    // 일반적인 쓰기
                    else
                    {
                        Chunk NewChunk(LastPtr, LastLength, CurEndian);
                             if(LastLength == 1) {auto v = va_arg(args, va_uint08); NewChunk.Save((bytes) &v, sizeof(v));}
                        else if(LastLength == 2) {auto v = va_arg(args, va_uint16); NewChunk.Save((bytes) &v, sizeof(v));}
                        else if(LastLength == 4) {auto v = va_arg(args, va_uint32); NewChunk.Save((bytes) &v, sizeof(v));}
                        else if(LastLength == 8) {auto v = va_arg(args, va_uint64); NewChunk.Save((bytes) &v, sizeof(v));}
                        else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[-1] - format);
                    }
                }
                else if(*Pos == '[')
                {
                         if(Pos[1] == 'b' && Pos[2] == 'e' && Pos[3] == ']') CurEndian = true;
                    else if(Pos[1] == 'l' && Pos[2] == 'e' && Pos[3] == ']') CurEndian = false;
                    else return BLIK_SERIAL_ERROR_BROKEN_FORMAT - (&Pos[1] - format);
                    Pos += 4;
                }
                else Pos++;
            }
            return mWriteStream.Count();
        }

        void WriteFlush(sint32 type)
        {
            // 쓰기스트림의 처리
            if(0 < mWriteStream.Count())
            {
                static uint08s NewEncodedData;
                NewEncodedData.SubtractionAll();
                mEnc(&mWriteStream[0], mWriteStream.Count(), NewEncodedData, type);
                mWriteStream.SubtractionAll();

                if(0 < NewEncodedData.Count())
                {
                    mSerial->write((const char*) &NewEncodedData[0], NewEncodedData.Count());
                    mSerial->flush();
                }
            }
        }

    private:
        static sint32 DefaultSerialDecode(bytes data, sint32 length, uint08s& outdata, sint32* outtype)
        {
            Memory::Copy(outdata.AtDumping(0, length), data, length);
            return length;
        }

        static void DefaultSerialEncode(bytes data, sint32 length, uint08s& outdata, sint32 type)
        {
            Memory::Copy(outdata.AtDumping(0, length), data, length);
        }
    };

    class CameraSurface : public QAbstractVideoSurface
    {
        Q_OBJECT

    private:
        QCamera mCamera;
        id_mutex mMutex;
        sint32 mLastWidth;
        sint32 mLastHeight;
        QVideoFrame::PixelFormat mPixelFormat;
        bool mNeedFlip;
        uint08s mLastImage;

    public:
        CameraSurface(const QCameraInfo& info) : mCamera(info)
        {
            mMutex = Mutex::Open();
            mLastWidth = 0;
            mLastHeight = 0;
            mNeedFlip = false;
        }
        ~CameraSurface()
        {
            StopCamera();
            Mutex::Close(mMutex);
        }

    public:
        void StartCamera() {mCamera.start();}
        void StopCamera() {mCamera.stop();}
        const QCameraViewfinderSettings GetSettings() {return mCamera.viewfinderSettings();}
        void SetSettings(const QCameraViewfinderSettings& settings) {mCamera.setViewfinderSettings(settings);}
        const QList<QCameraViewfinderSettings> GetSupportedAllSettings() {return mCamera.supportedViewfinderSettings();}

    protected:
        void StartPreview(bool preview) {mCamera.setViewfinder(this);}
        void StopPreview() {mCamera.setViewfinder((QAbstractVideoSurface*) nullptr);}
        virtual bool CaptureEnabled() {return false;}
        virtual Bmp::bitmappixel* GetBuffer(sint32 width, sint32 height) {return nullptr;}
        virtual void BufferFlush() {}
        virtual void AddPictureShotCount() {}
        virtual void AddPreviewShotCount() {}

    protected:
        void DecodeImage(sint32& width, sint32& height, uint08s& bits)
        {
            Mutex::Lock(mMutex);
            if(0 < mLastImage.Count())
            {
                width = mLastWidth;
                height = mLastHeight;
                bits.SubtractionAll();
                if(mNeedFlip)
                {
                    switch(mPixelFormat)
                    {
                    case QVideoFrame::Format_RGB32:
                        ToARGB32<QVideoFrame::Format_RGB32, true>((bytes) &mLastImage[0], width, height, 4 * width,
                            bits.AtDumpingAdded(4 * width * height));
                        break;
                    case QVideoFrame::Format_BGR32:
                        ToARGB32<QVideoFrame::Format_BGR32, true>((bytes) &mLastImage[0], width, height, 4 * width,
                            bits.AtDumpingAdded(4 * width * height));
                        break;
                    }
                }
                else switch(mPixelFormat)
                {
                case QVideoFrame::Format_RGB32:
                    ToARGB32<QVideoFrame::Format_RGB32, false>((bytes) &mLastImage[0], width, height, 4 * width,
                        bits.AtDumpingAdded(4 * width * height));
                    break;
                case QVideoFrame::Format_BGR32:
                    ToARGB32<QVideoFrame::Format_BGR32, false>((bytes) &mLastImage[0], width, height, 4 * width,
                        bits.AtDumpingAdded(4 * width * height));
                    break;
                }
                //단편화방지차원: mLastImage.Clear();
            }
            Mutex::Unlock(mMutex);
        }

    private:
        virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
        {
            Q_UNUSED(handleType);
            return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_BGR32;
        }

    private slots:
        virtual bool present(const QVideoFrame& frame)
        {
            AddPictureShotCount();
            bool Result = false;
            if(frame.isValid() && CaptureEnabled())
            {
                // QVideoFrame::map함수의 버그에 따른 수동패치
                auto FrameType = frame.handleType();
                if(FrameType == QAbstractVideoBuffer::GLTextureHandle)
                {
                    GLuint textureId = frame.handle().toUInt();
                    QOpenGLContext* ctx = QOpenGLContext::currentContext();
                    QOpenGLFunctions* f = ctx->functions();

                    GLuint fbo = 0, prevFbo = 0;
                    f->glGenFramebuffers(1, &fbo);
                    f->glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*) &prevFbo);
                    f->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
                    f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
                    Mutex::Lock(mMutex);
                    {
                        mLastWidth = frame.width();
                        mLastHeight = frame.height();
                        mPixelFormat = frame.pixelFormat();
                        mNeedFlip = false;
                        mLastImage.SubtractionAll();
                        f->glReadPixels(0, 0, mLastWidth, mLastHeight, GL_RGBA, GL_UNSIGNED_BYTE,
                            mLastImage.AtDumpingAdded(4 * mLastWidth * mLastHeight));
                    }
                    Mutex::Unlock(mMutex);
                    BufferFlush();
                    f->glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
                    f->glDeleteFramebuffers(1, &fbo);
                    Result = true;
                }
                else
                {
                    QVideoFrame ClonedFrame(frame);
                    if(ClonedFrame.map(QAbstractVideoBuffer::ReadOnly))
                    {
                        Mutex::Lock(mMutex);
                        {
                            mLastWidth = ClonedFrame.width();
                            mLastHeight = ClonedFrame.height();
                            mPixelFormat = ClonedFrame.pixelFormat();
                            mNeedFlip = true;
                            mLastImage.SubtractionAll();
                            Memory::Copy(mLastImage.AtDumpingAdded(4 * mLastWidth * mLastHeight),
                                ClonedFrame.bits(), 4 * mLastWidth * mLastHeight);
                        }
                        Mutex::Unlock(mMutex);
                        BufferFlush();
                        ClonedFrame.unmap();
                        Result = true;
                    }
                }
            }
            return Result;
        }

    private:
        template<sint32 FORMAT, bool FLIP>
        void ToARGB32(bytes srcbits, const sint32 width, const sint32 height, const sint32 rowbytes, uint08* dstbits)
        {
            for(sint32 y = 0, yend = height; y < yend; ++y)
            {
                Bmp::bitmappixel* CurDst = (Bmp::bitmappixel*) &dstbits[rowbytes * y];
                bytes CurSrc = (FLIP)? &srcbits[rowbytes * (yend - 1 - y)] : &srcbits[rowbytes * y];
                for(sint32 x = 0, xend = width; x < xend; ++x)
                {
                    if(FORMAT == (sint32) QVideoFrame::Format_RGB32)
                    {
                        CurDst->a = 0xFF;
                        CurDst->r = CurSrc[2];
                        CurDst->g = CurSrc[1];
                        CurDst->b = CurSrc[0];
                        CurDst++; CurSrc += 4;
                    }
                    if(FORMAT == (sint32) QVideoFrame::Format_BGR32)
                    {
                        CurDst->a = 0xFF;
                        CurDst->r = CurSrc[0];
                        CurDst->g = CurSrc[1];
                        CurDst->b = CurSrc[2];
                        CurDst++; CurSrc += 4;
                    }
                }
            }
        }
    };

    #if BLIK_ANDROID
        class CameraSurfaceForAndroid
        {
        private:
            id_mutex mMutex;
            uint08s mLastImage;
            uint32 mTextureId;

        public:
            CameraSurfaceForAndroid(const QCameraInfo& info)
            {
                mMutex = Mutex::Open();
                mTextureId = -1;
                JNINativeMethod methods[] {
                    {"OnPictureTaken", "([BI)V", reinterpret_cast<void*>(OnPictureTaken)},
                    {"OnPreviewTaken", "([BIII)V", reinterpret_cast<void*>(OnPreviewTaken)}};
                QAndroidJniObject javaClass("com/blik2d/CameraManager");
                QAndroidJniEnvironment env;
                jclass objectClass = env->GetObjectClass(javaClass.object<jobject>());
                env->RegisterNatives(objectClass, methods, sizeof(methods) / sizeof(methods[0]));
                env->DeleteLocalRef(objectClass);
                SavedMe() = this;
            }
            ~CameraSurfaceForAndroid()
            {
                StopCamera();
                SavedMe() = nullptr;
                Mutex::Close(mMutex);
            }

        public:
            void StartCamera()
            {
                QOpenGLContext* ctx = QOpenGLContext::currentContext();
                BLIK_TRACE("StartCamera: GenTexture begin: 0x%08X", ctx);
                QOpenGLFunctions* f = ctx->functions();
                f->glGenTextures(1, &mTextureId);
                f->glBindTexture(GL_TEXTURE_2D, mTextureId);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
                BLIK_TRACE("StartCamera: GenTexture - mTextureId: %d", mTextureId);
                QAndroidJniObject::callStaticMethod<void>("com/blik2d/CameraManager", "init", "(I)V", mTextureId);
            }
            void StopCamera()
            {
                QAndroidJniObject::callStaticMethod<void>("com/blik2d/CameraManager", "quit", "()V");
                QOpenGLContext* ctx = QOpenGLContext::currentContext();
                BLIK_TRACE("StopCamera: DeleteTexture begin: 0x%08X, %d", ctx, mTextureId);
                QOpenGLFunctions* f = ctx->functions();
                f->glDeleteTextures(1, &mTextureId);
                mTextureId = -1;
                BLIK_TRACE("StopCamera: DeleteTexture done");
            }
            const QCameraViewfinderSettings GetSettings() {return QCameraViewfinderSettings();}
            void SetSettings(const QCameraViewfinderSettings& settings) {}
            const QList<QCameraViewfinderSettings> GetSupportedAllSettings() {return QList<QCameraViewfinderSettings>();}

        protected:
            void StartPreview(bool preview) {QAndroidJniObject::callStaticMethod<void>("com/blik2d/CameraManager", "play", "(I)V", (preview)? 1 : 2);}
            void StopPreview() {QAndroidJniObject::callStaticMethod<void>("com/blik2d/CameraManager", "stop", "()V");}
            virtual bool CaptureEnabled() {return false;}
            virtual Bmp::bitmappixel* GetBuffer(sint32 width, sint32 height) {return nullptr;}
            virtual void BufferFlush() {}
            virtual void AddPictureShotCount() {}
            virtual void AddPreviewShotCount() {}

        protected:
            void DecodeImage(sint32& width, sint32& height, uint08s& bits)
            {
                BLIK_TRACE("DecodeImage - Begin(%d)", mLastImage.Count());
                Mutex::Lock(mMutex);
                if(0 < mLastImage.Count())
                {
                    switch(*((uint32*) &mLastImage[0]))
                    {
                    case 'NV21':
                        {
                            BLIK_TRACE("DecodeImage(RAW8) - Mid-A");
                            width = *((sint32*) &mLastImage[4]);
                            height = *((sint32*) &mLastImage[8]);
                            bits.SubtractionAll();
                            Bmp::bitmappixel* Dst = (Bmp::bitmappixel*)
                                bits.AtDumpingAdded(sizeof(Bmp::bitmappixel) * width * height);
                            auto SrcY = (const uint08*) &mLastImage[12];
                            auto SrcUVBegin = (const uint08*) &mLastImage[12 + width * height];
                            const uint08* SrcUVPair[2] = {SrcUVBegin, SrcUVBegin};
                            for(sint32 y = 0; y < height; ++y)
                            {
                                const uint08*& SrcUV = SrcUVPair[y & 1];
                                for(sint32 x = 0; x < width; x += 2)
                                {
                                    const sint32 Y1 = *(SrcY++) & 0xFF;
                                    const sint32 Y2 = *(SrcY++) & 0xFF;
                                    const sint32 V = (*(SrcUV++) & 0xFF) - 128;
                                    const sint32 U = (*(SrcUV++) & 0xFF) - 128;
                                    const sint32 Y1Value = (sint32)(1.164f * (Y1 - 16));
                                    const sint32 Y2Value = (sint32)(1.164f * (Y2 - 16));
                                    const sint32 RValue = (sint32)(1.596f * V);
                                    const sint32 GValue = (sint32)(0.391f * U + 0.813f * V);
                                    const sint32 BValue = (sint32)(2.018f * U);
                                    Dst->a = 0xFF;
                                    Dst->r = (uint08) Math::Max(0, Math::Min(Y1Value + RValue, 255));
                                    Dst->g = (uint08) Math::Max(0, Math::Min(Y1Value - GValue, 255));
                                    Dst->b = (uint08) Math::Max(0, Math::Min(Y1Value + BValue, 255));
                                    Dst++;
                                    Dst->a = 0xFF;
                                    Dst->r = (uint08) Math::Max(0, Math::Min(Y2Value + RValue, 255));
                                    Dst->g = (uint08) Math::Max(0, Math::Min(Y2Value - GValue, 255));
                                    Dst->b = (uint08) Math::Max(0, Math::Min(Y2Value + BValue, 255));
                                    Dst++;
                                }
                            }
                            BLIK_TRACE("DecodeImage(RAW8) - Mid-B");
                        }
                        break;
                    case 'JPEG':
                        BLIK_TRACE("DecodeImage(JPEG)");
                        if(id_bitmap NewBitmap = AddOn::Jpg::ToBmp(&mLastImage[4], mLastImage.Count()))
                        {
                            BLIK_TRACE("DecodeImage(JPEG) - Mid-A");
                            width = Bmp::GetWidth(NewBitmap);
                            height = Bmp::GetHeight(NewBitmap);
                            bits.SubtractionAll();
                            Bmp::bitmappixel* Dst = (Bmp::bitmappixel*)
                                bits.AtDumpingAdded(sizeof(Bmp::bitmappixel) * width * height);
                            auto Src = (const Bmp::bitmappixel*) Bmp::GetBits(NewBitmap);
                            for(sint32 y = 0; y < height; ++y)
                                Memory::Copy(&Dst[width * y], &Src[width * (height - 1 - y)],
                                    sizeof(Bmp::bitmappixel) * width);
                            Bmp::Remove(NewBitmap);
                            BLIK_TRACE("DecodeImage(JPEG) - Mid-B");
                        }
                        break;
                    }
                    //단편화방지차원: mLastImage.Clear();
                }
                else
                {
                    width = 1;
                    height = 1;
                    bits.SubtractionAll();
                    Bmp::bitmappixel* Dst = (Bmp::bitmappixel*)
                        bits.AtDumpingAdded(sizeof(Bmp::bitmappixel) * width * height);
                    Dst->a = 0xFF;
                    Dst->r = 0xFF;
                    Dst->g = 0x00;
                    Dst->b = 0x00;
                }
                Mutex::Unlock(mMutex);
                BLIK_TRACE("DecodeImage - End(%d)", bits.Count());
            }

        private:
            static CameraSurfaceForAndroid*& SavedMe()
            {
                static CameraSurfaceForAndroid* _ = nullptr;
                return _;
            }
            static void OnPictureTaken(JNIEnv* env, jobject thiz, jbyteArray data, jint length)
            {
                BLIK_TRACE("OnPictureTaken Begin");
                bytes paramData = (bytes) env->GetByteArrayElements(data, nullptr);
                CameraSurfaceForAndroid* Me = SavedMe();
                BLIK_TRACE("OnPictureTaken - paramData: %08X, length: %d, Me: %08X", paramData, length, Me);
                if(Me)
                {
                    Me->AddPictureShotCount();
                    if(Me->CaptureEnabled())
                    if(paramData && 0 < length)
                    {
                        if(16 <= length)
                        BLIK_TRACE("OnPictureTaken - %08X %08X %08X %08X... (%d)",
                            ((sint32*) paramData)[0], ((sint32*) paramData)[1],
                            ((sint32*) paramData)[2], ((sint32*) paramData)[3], length);
                        Mutex::Lock(Me->mMutex);
                        Me->mLastImage.SubtractionAll();
                        const uint32 TypeCode = 'JPEG';
                        Memory::Copy(Me->mLastImage.AtDumpingAdded(4), &TypeCode, 4);
                        Memory::Copy(Me->mLastImage.AtDumpingAdded(length), paramData, length);
                        Mutex::Unlock(Me->mMutex);
                        Me->BufferFlush();
                    }
                }
                env->ReleaseByteArrayElements(data, (jbyte*) paramData, JNI_ABORT);
                BLIK_TRACE("OnPictureTaken End");
            }
            static void OnPreviewTaken(JNIEnv* env, jobject thiz, jbyteArray data, jint length, jint width, jint height)
            {
                BLIK_TRACE("OnPreviewTaken Begin");
                bytes paramData = (bytes) env->GetByteArrayElements(data, nullptr);
                CameraSurfaceForAndroid* Me = SavedMe();
                BLIK_TRACE("OnPreviewTaken - paramData: %08X, length: %d, Me: %08X", paramData, length, Me);
                if(Me)
                {
                    Me->AddPreviewShotCount();
                    if(Me->CaptureEnabled())
                    if(paramData && 0 < length)
                    {
                        if(16 <= length)
                        BLIK_TRACE("OnPreviewTaken - %08X %08X %08X %08X... (%d)",
                            ((sint32*) paramData)[0], ((sint32*) paramData)[1],
                            ((sint32*) paramData)[2], ((sint32*) paramData)[3], length);
                        Mutex::Lock(Me->mMutex);
                        Me->mLastImage.SubtractionAll();
                        const uint32 TypeCode = 'NV21';
                        Memory::Copy(Me->mLastImage.AtDumpingAdded(4), &TypeCode, 4);
                        Memory::Copy(Me->mLastImage.AtDumpingAdded(4), &width, 4);
                        Memory::Copy(Me->mLastImage.AtDumpingAdded(4), &height, 4);
                        Memory::Copy(Me->mLastImage.AtDumpingAdded(length), paramData, length);
                        Mutex::Unlock(Me->mMutex);
                        Me->BufferFlush();
                    }
                }
                env->ReleaseByteArrayElements(data, (jbyte*) paramData, JNI_ABORT);
                BLIK_TRACE("OnPreviewTaken End");
            }
        };
        typedef CameraSurfaceForAndroid CameraSurfaceClass;
    #else
        typedef CameraSurface CameraSurfaceClass;
    #endif

    class CameraService : public CameraSurfaceClass
    {
    private:
        enum CaptureOrder {CaptureOrder_None, CaptureOrder_ShotOnly, CaptureOrder_ShotAndStop, CaptureOrder_NeedStop};
        id_mutex mMutex;
        bool mStartPreview;
        CaptureOrder mCaptureOrder;
        sint32 mDecodedWidth;
        sint32 mDecodedHeight;
        uint08s mDecodedBits;
        bool mUpdateForImage;
        bool mUpdateForBitmap;
        uint64 mUpdateTimeMsec;
        sint32 mPictureShotCount;
        sint32 mPreviewShotCount;

    public:
        CameraService(const QCameraInfo& info) : CameraSurfaceClass(info)
        {
            mMutex = Mutex::Open();
            mStartPreview = false;
            mCaptureOrder = CaptureOrder_None;
            mDecodedWidth = 0;
            mDecodedHeight = 0;
            mUpdateForImage = false;
            mUpdateForBitmap = false;
            mUpdateTimeMsec = 0;
            mPictureShotCount = 0;
            mPreviewShotCount = 0;
        }
        ~CameraService()
        {
            Mutex::Close(mMutex);
        }

    private:
        virtual bool CaptureEnabled() override
        {
            bool Result = false;
            Mutex::Lock(mMutex);
            {
                Result = (mCaptureOrder == CaptureOrder_ShotOnly
                    || mCaptureOrder == CaptureOrder_ShotAndStop);
            }
            Mutex::Unlock(mMutex);
            return Result;
        }
        virtual void BufferFlush() override
        {
            Mutex::Lock(mMutex);
            {
                if(mCaptureOrder == CaptureOrder_ShotOnly)
                    mCaptureOrder = CaptureOrder_None;
                else if(mCaptureOrder == CaptureOrder_ShotAndStop)
                    mCaptureOrder = CaptureOrder_NeedStop;
                mUpdateForImage = true;
                mUpdateForBitmap = true;
                mUpdateTimeMsec = Platform::Utility::CurrentTimeMsec();
            }
            Mutex::Unlock(mMutex);
        }
        virtual void AddPictureShotCount() override {mPictureShotCount++;}
        virtual void AddPreviewShotCount() override {mPreviewShotCount++;}

    public:
        void Capture(bool preview, bool needstop)
        {
            Mutex::Lock(mMutex);
            {
                mCaptureOrder = (needstop)? CaptureOrder_ShotAndStop : CaptureOrder_ShotOnly;
                mStartPreview = true;
                StartPreview(preview);
            }
            Mutex::Unlock(mMutex);
        }
        void GetCapturedImage(QPixmap& pixmap, sint32 maxwidth, sint32 maxheight)
        {
            Mutex::Lock(mMutex);
            {
                if(mCaptureOrder == CaptureOrder_NeedStop)
                {
                    mCaptureOrder = CaptureOrder_None;
                    if(mStartPreview)
                    {
                        mStartPreview = false;
                        StopPreview();
                    }
                }
                if(mUpdateForImage)
                {
                    mUpdateForImage = false;
                    if(mUpdateForBitmap) DecodeImage(mDecodedWidth, mDecodedHeight, mDecodedBits);
                    QImage NewImage(&mDecodedBits[0], mDecodedWidth, mDecodedHeight, QImage::Format_ARGB32);
                    if(maxwidth == -1 && maxheight == -1)
                    {maxwidth = mDecodedWidth; maxheight = mDecodedHeight;}
                    else if(maxwidth == -1) maxwidth = mDecodedWidth * maxheight / mDecodedHeight;
                    else if(maxheight == -1) maxheight = mDecodedHeight * maxwidth / mDecodedWidth;
                    if(maxwidth < mDecodedWidth || maxheight < mDecodedHeight)
                        pixmap.convertFromImage(NewImage.scaled(
                            Math::Min(maxwidth, mDecodedWidth), Math::Min(maxheight, mDecodedHeight)));
                    else pixmap.convertFromImage(NewImage);
                    //단편화방지차원: if(!mUpdateForBitmap) mDecodedBits.Clear();
                }
            }
            Mutex::Unlock(mMutex);
        }
        void GetCapturedBitmap(Image& image, bool vflip)
        {
            Mutex::Lock(mMutex);
            {
                if(mCaptureOrder == CaptureOrder_NeedStop)
                {
                    mCaptureOrder = CaptureOrder_None;
                    if(mStartPreview)
                    {
                        mStartPreview = false;
                        StopPreview();
                    }
                }
                if(mUpdateForBitmap)
                {
                    mUpdateForBitmap = false;
                    if(mUpdateForImage) DecodeImage(mDecodedWidth, mDecodedHeight, mDecodedBits);
                    image.LoadBitmapFromBits(&mDecodedBits[0], mDecodedWidth, mDecodedHeight, 32, vflip);
                    //단편화방지차원: if(!mUpdateForImage) mDecodedBits.Clear();
                }
            }
            Mutex::Unlock(mMutex);
        }
        size64 GetCapturedSize() const
        {
            size64 Result = {0, 0};
            Mutex::Lock(mMutex);
            {
                Result.w = mDecodedWidth;
                Result.h = mDecodedHeight;
            }
            Mutex::Unlock(mMutex);
            return Result;
        }
        uint64 GetCapturedTimeMS() const
        {
            uint64 Result = 0;
            Mutex::Lock(mMutex);
            {
                Result = mUpdateTimeMsec;
            }
            Mutex::Unlock(mMutex);
            return Result;
        }
        sint32 GetPictureShotCount() const
        {
            return mPictureShotCount;
        }
        sint32 GetPreviewShotCount() const
        {
            return mPreviewShotCount;
        }
    };

    class CameraClass
    {
    private:
        sint32 mRefCount;
        QCameraInfo mCameraInfo;
        CameraService* mCameraService;
        QPixmap mLastPixmap;
        Image mLastImage;

    public:
        static Strings GetList(String* spec)
        {
            Strings Result;
            Context SpecCollector;
            const QList<QCameraInfo>& AllCameras = QCameraInfo::availableCameras();
            foreach(const auto& CurCamera, AllCameras)
            {
                String CurName = CurCamera.description().toUtf8().constData();
                if(CurName.Length() == 0)
                    CurName = CurCamera.deviceName().toUtf8().constData();
                Result.AtAdding() = CurName;
                if(spec)
                {
                    Context& NewChild = SpecCollector.At(SpecCollector.LengthOfIndexable());
                    NewChild.At("description").Set(CurCamera.description().toUtf8().constData());
                    NewChild.At("devicename").Set(CurCamera.deviceName().toUtf8().constData());
                    NewChild.At("position").Set(String::FromInteger(CurCamera.position()));
                    NewChild.At("orientation").Set(String::FromInteger(CurCamera.orientation()));
                }
            }
            if(spec)
                *spec = SpecCollector.SaveJson(*spec);
            return Result;
        }

    public:
        CameraClass(chars name, sint32 width, sint32 height)
        {
            mRefCount = 1;
            mCameraService = nullptr;
            const QList<QCameraInfo>& AllCameraInfos = QCameraInfo::availableCameras();
            foreach(const auto& CurCameraInfo, AllCameraInfos)
            {
                if(*name == '\0' || CurCameraInfo.description() == name || CurCameraInfo.deviceName() == name)
                {
                    mCameraInfo = CurCameraInfo;
                    mCameraService = new CameraService(mCameraInfo);

                    sint32 BestValue = 0, SavedWidth = width, SavedHeight = height;
                    double SavedMinFR = -1, SavedMaxFR = -1;
                    auto AllSettings = mCameraService->GetSupportedAllSettings();
                    BLIK_TRACE("Created Camera: %s [Included %d settings]",
                        CurCameraInfo.description().toUtf8().constData(), AllSettings.size());
                    BLIK_TRACE(" - Setting Count: %d", AllSettings.size());
                    foreach(const auto& CurSetting, AllSettings)
                    {
                        const sint32 CurWidth = CurSetting.resolution().width();
                        const sint32 CurHeight = CurSetting.resolution().height();
                        const double CurMinFR = CurSetting.minimumFrameRate();
                        const double CurMaxFR = CurSetting.maximumFrameRate();
                        BLIK_TRACE(" - 1.Supported Resolution: %dx%d", CurWidth, CurHeight);
                        BLIK_TRACE(" - 2.Supported PixelFormat: %d", (sint32) CurSetting.pixelFormat());
                        BLIK_TRACE(" - 3.Supported FrameRate: %f~%f", (float) CurMinFR, (float) CurMaxFR);
                        bool IsFinded = false;
                        if(width == -1 && height == -1)
                        {
                            if(BestValue <= CurWidth * CurHeight)
                            {
                                BestValue = CurWidth * CurHeight;
                                IsFinded = true;
                            }
                        }
                        else if(width == -1)
                        {
                            if(height == CurHeight && BestValue <= CurWidth)
                            {
                                BestValue = CurWidth;
                                IsFinded = true;
                            }
                        }
                        else if(height == -1)
                        {
                            if(width == CurWidth && BestValue <= CurHeight)
                            {
                                BestValue = CurHeight;
                                IsFinded = true;
                            }
                        }
                        else if(width == CurWidth && height == CurHeight)
                            IsFinded = true;
                        if(IsFinded)
                        {
                            SavedWidth = CurWidth;
                            SavedHeight = CurHeight;
                            SavedMinFR = SavedMinFR;
                            SavedMaxFR = SavedMaxFR;
                        }
                    }

                    if(SavedWidth != -1 && SavedHeight != -1)
                    {
                        QCameraViewfinderSettings NewSettings = mCameraService->GetSettings();
                        NewSettings.setResolution(SavedWidth, SavedHeight);
                        if(SavedMinFR != -1) NewSettings.setMinimumFrameRate(SavedMinFR);
                        if(SavedMaxFR != -1) NewSettings.setMaximumFrameRate(SavedMaxFR);
                        mCameraService->SetSettings(NewSettings);
                    }
                    mCameraService->StartCamera();
                    break;
                }
            }
        }
        ~CameraClass()
        {
            delete mCameraService;
        }

    public:
        CameraClass* AddRef()
        {
            mRefCount++;
            return this;
        }
        bool SubRef()
        {
            return (--mRefCount == 0);
        }
        bool IsValid() const
        {
            return (mCameraService != nullptr);
        }
        void Capture(bool preview, bool needstop)
        {
            if(mCameraService)
                mCameraService->Capture(preview, needstop);
        }
        id_image_read LastCapturedImage(sint32 maxwidth, sint32 maxheight)
        {
            if(mCameraService)
            {
                mCameraService->GetCapturedImage(mLastPixmap, maxwidth, maxheight);
                return (id_image_read) &mLastPixmap;
            }
            return nullptr;
        }
        id_bitmap_read LastCapturedBitmap(bool vflip)
        {
            if(mCameraService)
            {
                mCameraService->GetCapturedBitmap(mLastImage, vflip);
                return mLastImage.GetBitmap();
            }
            return nullptr;
        }
        size64 LastCapturedSize() const
        {
            if(mCameraService)
                return mCameraService->GetCapturedSize();
            return {0, 0};
        }
        uint64 LastCapturedTimeMS() const
        {
            if(mCameraService)
                return mCameraService->GetCapturedTimeMS();
            return 0;
        }
        sint32 TotalPictureShotCount() const
        {
            if(mCameraService)
                return mCameraService->GetPictureShotCount();
            return 0;
        }
        sint32 TotalPreviewShotCount() const
        {
            if(mCameraService)
                return mCameraService->GetPreviewShotCount();
            return 0;
        }
    };

    class MicrophoneClass : public QAudioProbe
    {
        Q_OBJECT

    private:
        class Data
        {
        public:
            Data() {}
            ~Data() {}
        public:
            Data(const Data& rhs) {operator=(rhs);}
            Data& operator=(const Data& rhs)
            {
                mPcm = rhs.mPcm;
                mTimeMsec = rhs.mTimeMsec;
                return *this;
            }
            operator void*() const {return nullptr;}
            bool operator!() const {return (mPcm.Count() == 0);}
        public:
            uint08s mPcm;
            uint64 mTimeMsec;
        };

    private:
        QAudioRecorder* mRecorder;
        QAudioEncoderSettings mAudioSettings;
        const sint32 mMaxQueueCount;
        Queue<Data> mDataQueue;
        Data mLastData;

    public:
        static Strings GetList(String* spec)
        {
            Strings Result;
            Context SpecCollector;
            const QList<QAudioDeviceInfo>& AllMicrophones = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
            foreach(const auto& CurMicrophone, AllMicrophones)
            {
                String CurName = CurMicrophone.deviceName().toUtf8().constData();
                Result.AtAdding() = CurName;
                if(spec)
                {
                    Context& NewChild = SpecCollector.At(SpecCollector.LengthOfIndexable());
                    NewChild.At("devicename").Set(CurName);
                    const auto& AllByteOrders = CurMicrophone.supportedByteOrders();
                    foreach(const auto& CurByteOrder, AllByteOrders)
                    {
                        Context& NewChild2 = NewChild.At("byteorders").At(NewChild.At("byteorders").LengthOfIndexable());
                        if(CurByteOrder == QAudioFormat::BigEndian) NewChild2.Set("BigEndian");
                        else if(CurByteOrder == QAudioFormat::LittleEndian) NewChild2.Set("LittleEndian");
                    }
                    const auto& AllChannelCounts = CurMicrophone.supportedChannelCounts();
                    foreach(const auto& CurChannelCount, AllChannelCounts)
                    {
                        Context& NewChild2 = NewChild.At("channelcounts").At(NewChild.At("channelcounts").LengthOfIndexable());
                        NewChild2.Set(String::FromInteger(CurChannelCount));
                    }
                    const auto& AllCodecs = CurMicrophone.supportedCodecs();
                    foreach(const auto& CurCodec, AllCodecs)
                    {
                        Context& NewChild2 = NewChild.At("codecs").At(NewChild.At("codecs").LengthOfIndexable());
                        NewChild2.Set(CurCodec.toUtf8().constData());
                    }
                    const auto& AllSampleRates = CurMicrophone.supportedSampleRates();
                    foreach(const auto& CurSampleRate, AllSampleRates)
                    {
                        Context& NewChild2 = NewChild.At("samplerates").At(NewChild.At("samplerates").LengthOfIndexable());
                        NewChild2.Set(String::FromInteger(CurSampleRate));
                    }
                    const auto& AllSampleSizes = CurMicrophone.supportedSampleSizes();
                    foreach(const auto& CurSampleSize, AllSampleSizes)
                    {
                        Context& NewChild2 = NewChild.At("samplesizes").At(NewChild.At("samplesizes").LengthOfIndexable());
                        NewChild2.Set(String::FromInteger(CurSampleSize));
                    }
                    const auto& AllSampleTypes = CurMicrophone.supportedSampleTypes();
                    foreach(const auto& CurSampleType, AllSampleTypes)
                    {
                        Context& NewChild2 = NewChild.At("sampletypes").At(NewChild.At("sampletypes").LengthOfIndexable());
                        if(CurSampleType == QAudioFormat::Unknown) NewChild2.Set("Unknown");
                        else if(CurSampleType == QAudioFormat::SignedInt) NewChild2.Set("SignedInt");
                        else if(CurSampleType == QAudioFormat::UnSignedInt) NewChild2.Set("UnSignedInt");
                        else if(CurSampleType == QAudioFormat::Float) NewChild2.Set("Float");
                    }
                }
            }
            if(spec)
                *spec = SpecCollector.SaveJson(*spec);
            return Result;
        }

    public:
        MicrophoneClass(chars name, sint32 maxcount) : mMaxQueueCount(maxcount)
        {
            mRecorder = new QAudioRecorder();
            QString SelectedInput = "";
            if(*name != '\0')
            {
                QStringList Inputs = mRecorder->audioInputs();
                foreach(QString Input, Inputs)
                    if(!String::Compare(name, Input.toUtf8().constData()))
                        SelectedInput = Input;
            }
            else SelectedInput = mRecorder->defaultAudioInput();
            if(SelectedInput.length() == 0)
            {
                delete mRecorder;
                mRecorder = nullptr;
                return;
            }
            mRecorder->setAudioInput(SelectedInput);

            mAudioSettings = mRecorder->audioSettings();
            mAudioSettings.setCodec("audio/pcm");
            mAudioSettings.setBitRate(128000);
            mAudioSettings.setChannelCount(2);
            mAudioSettings.setSampleRate(44100);
            mAudioSettings.setQuality(QMultimedia::HighQuality);
            mAudioSettings.setEncodingMode(QMultimedia::ConstantBitRateEncoding);
            mRecorder->setAudioSettings(mAudioSettings);

            connect(this, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(processBuffer(QAudioBuffer)));
            setSource(mRecorder);
            mRecorder->record();
        }
        ~MicrophoneClass()
        {
            setSource((QMediaRecorder*) nullptr);
            delete mRecorder;
            while(0 < mDataQueue.Count())
                mDataQueue.Dequeue();
        }

    public:
        bool IsValid()
        {
            return (mRecorder != nullptr);
        }
        bool TryLastData()
        {
            if(0 < mDataQueue.Count())
            {
                mLastData = mDataQueue.Dequeue();
                return true;
            }
            return false;
        }
        const uint08s& GetLastData(uint64* timems) const
        {
            if(timems) *timems = mLastData.mTimeMsec;
            return mLastData.mPcm;
        }
        const QAudioEncoderSettings& GetAudioSettings() const
        {return mAudioSettings;}

    private slots:
        virtual void processBuffer(const QAudioBuffer& buffer)
        {
            // 한계처리
            while(mMaxQueueCount < mDataQueue.Count())
                mDataQueue.Dequeue();
            // 데이터적재
            if(buffer.isValid())
            {
                Data NewData;
                Memory::Copy(NewData.mPcm.AtDumpingAdded(buffer.byteCount()), buffer.constData(), buffer.byteCount());
                NewData.mTimeMsec = Platform::Utility::CurrentTimeMsec();
                mDataQueue.Enqueue(NewData);
            }
        }
    };

#elif defined(_MSC_VER) && defined(QT_DLL)

    // 플랫폼매크로를 변경하였을때 아래 코드가 없으면
    // Qt는 moc를 다시 빌드하지 않아 링크에러를 유발.
    #include <QtWidgets>
    #include <QMainWindow>
    #include <QHostInfo>
    #include <QTcpSocket>
    #include <QUdpSocket>
    #include <QTcpServer>
    #include <QGLWidget>
    #include <QGLFunctions>
    #include <QGLShaderProgram>
    #include <QAbstractVideoSurface>
    #include <QAudioProbe>
    #include <QWebEngineView>
    class ViewAPI : public QObject {Q_OBJECT};
    class GenericView : public QFrame {Q_OBJECT};
    class MainViewGL : public QGLWidget {Q_OBJECT};
    class MainViewMDI : public QMdiArea {Q_OBJECT};
    class MainWindow : public QMainWindow {Q_OBJECT private slots: void OnSlot() {}};
    class EditTracker : public QLineEdit {Q_OBJECT};
    class ListTracker : public QListWidget {Q_OBJECT private slots: void onItemPressed(QListWidgetItem* item) {}};
    class VideoSurface : public QAbstractVideoSurface {Q_OBJECT};
    class ThreadClass : public QThread {Q_OBJECT};
    class TCPAgent : public QTcpServer {Q_OBJECT};
    class CameraSurface : public QAbstractVideoSurface {Q_OBJECT};
    class MicrophoneClass : public QAudioProbe {Q_OBJECT};
    class WebViewPrivate : public QWebEngineView {Q_OBJECT};

#endif
