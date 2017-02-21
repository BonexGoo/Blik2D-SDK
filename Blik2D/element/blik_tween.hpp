#pragma once
#include <element/blik_point.hpp>

#include <functional>

namespace BLIK
{
    typedef std::function<bool(bool needRedraw)> TweenUpdateCB;

    //! \brief 1D트윈
    class Tween1D : Updater
    {
    public:
        Tween1D(Updater* neighbor, float value = 0,
            TweenUpdateCB cb = [](bool needRedraw){return needRedraw;});
        Tween1D(const Tween1D& rhs);
        virtual ~Tween1D() override;

    public:
        Tween1D& operator=(const Tween1D& rhs);
        inline float value() const {return m_value;}

    public:
        void Reset(float value);
        void MoveTo(float value, float second);
        void SetUpdateCB(TweenUpdateCB cb);

    protected:
        virtual bool UpdateForRender() override;
        virtual bool UpdateForTick() override;

    private:
        float m_value;
        float m_valueStart;
        float m_valueEnd;
        uint64 m_timeStart;
        uint64 m_timeEnd;
        TweenUpdateCB m_updateCB;
        bool m_needWakeUp;
    };
    typedef Array<Tween1D> Tween1Ds;

    //! \brief 2D트윈
    class Tween2D : Updater
    {
    public:
        Tween2D(Updater* neighbor);
        Tween2D(const Tween2D& rhs);
        virtual ~Tween2D() override;

    public:
        Tween2D& operator=(const Tween2D& rhs);
        inline float x() const {return m_curpos.x;}
        inline float y() const {return m_curpos.y;}

    public:
        void Reset(float x, float y);
        void ResetPathes();
        void MoveTo(float x, float y, float velocity);
        void JumpTo(float x, float y);

    protected:
        virtual bool UpdateForRender() override;
        virtual bool UpdateForTick() override;

    private:
        Queue<buffer> m_pathes;
        buffer m_curpath;
        Point m_curpos;
        Point m_lastpos;
    };
    typedef Array<Tween2D> Tween2Ds;
}
