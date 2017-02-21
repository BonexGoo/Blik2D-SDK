#include <blik.hpp>
#include "blik_tween.hpp"

#include <platform/blik_platform.hpp>

namespace BLIK
{
    Tween1D::Tween1D(Updater* neighbor, float value, TweenUpdateCB cb) : Updater(neighbor)
    {
        Reset(value);
        m_updateCB = cb;
        m_needWakeUp = false;
    }

    Tween1D::Tween1D(const Tween1D& rhs)
    {
        operator=(rhs);
    }

    Tween1D::~Tween1D()
    {
        Reset(0);
    }

    Tween1D& Tween1D::operator=(const Tween1D& rhs)
    {
        const float OldValue = m_value;
        m_value = rhs.m_value;
        m_valueStart = rhs.m_valueStart;
        m_valueEnd = rhs.m_valueEnd;
        m_timeStart = rhs.m_timeStart;
        m_timeEnd = rhs.m_timeEnd;
        m_updateCB = rhs.m_updateCB;
        m_needWakeUp = (m_value != OldValue);
        return *this;
    }

    void Tween1D::Reset(float value)
    {
        const float OldValue = m_value;
        m_value = value;
        m_valueStart = value;
        m_valueEnd = value;
        m_timeStart = 0;
        m_timeEnd = 0;
        m_needWakeUp = (m_value != OldValue);
    }

    void Tween1D::MoveTo(float value, float second)
    {
        m_valueStart = m_value;
        m_valueEnd = value;
        m_timeStart = Platform::Utility::CurrentTimeMS();
        m_timeEnd = m_timeStart + (uint64) (Math::MaxF(0, second) * 1000);
        m_needWakeUp = (m_value != m_valueEnd);
    }

    void Tween1D::SetUpdateCB(TweenUpdateCB cb)
    {
        BLIK_ASSERT("cb는 nullptr가 될 수 없습니다", cb != nullptr);
        m_updateCB = cb;
    }

    bool Tween1D::UpdateForRender()
    {
        m_needWakeUp = false;
        const uint64 CurTime = Platform::Utility::CurrentTimeMS();
        const uint64 OldValue = m_value;
        if(m_timeEnd < CurTime) m_value = m_valueEnd;
        else if(CurTime < m_timeStart) m_value = m_valueStart;
        else m_value = m_valueStart + (m_valueEnd - m_valueStart)
            * (CurTime - m_timeStart) / (m_timeEnd - m_timeStart);
        return m_updateCB(m_value != OldValue);
    }

    bool Tween1D::UpdateForTick()
    {
        const bool NeedUpdate = m_needWakeUp;
        m_needWakeUp = false;
        return NeedUpdate;
    }

    Tween2D::Tween2D(Updater* neighbor) : Updater(neighbor)
    {
        m_curpath = nullptr;
    }

    Tween2D::Tween2D(const Tween2D& rhs)
    {
        operator=(rhs);
    }

    Tween2D::~Tween2D()
    {
        Reset(0, 0);
    }

    Tween2D& Tween2D::operator=(const Tween2D& rhs)
    {
        Reset(rhs.m_curpos.x, rhs.m_curpos.y);
        return *this;
    }

    void Tween2D::Reset(float x, float y)
    {
        m_curpos.x = x;
        m_curpos.y = y;
        ResetPathes();
    }

    void Tween2D::ResetPathes()
    {
        Buffer::Free(m_curpath);
        m_curpath = nullptr;
        while(buffer RemBuffer = m_pathes.Dequeue())
            Buffer::Free(RemBuffer);
        m_lastpos.x = m_curpos.x;
        m_lastpos.y = m_curpos.y;
    }

    class Tween2DPath
    {
    public:
        Tween2DPath()
        {
            m_count = 0;
            m_step = 0;
        }
        ~Tween2DPath()
        {
        }

        Tween2DPath& operator=(const Tween2DPath& rhs)
        {
            m_begin = rhs.m_begin;
            m_end = rhs.m_end;
            m_count = rhs.m_count;
            m_step = rhs.m_step;
            return *this;
        }

    public:
        void Init(const Point& begin, const Point& end, float velocity)
        {
            m_begin = begin;
            m_end = end;
            m_count = Math::Max(1, (sint32) (Math::Distance(begin.x, begin.y, end.x, end.y) / velocity));
            m_step = 0;
        }

        void Init(const Point& begin, const Point& end)
        {
            m_begin = begin;
            m_end = end;
            m_count = 1;
            m_step = 0;
        }

        const Point NextStep(buffer& This)
        {
            m_step++;
            if(m_step == m_count)
            {
                const Point SavedPos(m_end);
                Buffer::Free(This);
                This = nullptr;
                return SavedPos;
            }
            const float NewX = m_begin.x + (m_end.x - m_begin.x) * m_step / m_count;
            const float NewY = m_begin.y + (m_end.y - m_begin.y) * m_step / m_count;
            return Point(NewX, NewY);
        }

    private:
        Point m_begin;
        Point m_end;
        sint32 m_count;
        sint32 m_step;
    };

    void Tween2D::MoveTo(float x, float y, float velocity)
    {
        Point NewPos = Point(x, y);
        Tween2DPath* NewPath = (Tween2DPath*) Buffer::Alloc<Tween2DPath, datatype_class_canmemcpy>(BLIK_DBG 1);
        NewPath->Init(m_lastpos, NewPos, velocity);
        m_pathes.Enqueue((buffer) NewPath);
        m_lastpos = NewPos;
    }

    void Tween2D::JumpTo(float x, float y)
    {
        Point NewPos = Point(x, y);
        Tween2DPath* NewPath = (Tween2DPath*) Buffer::Alloc<Tween2DPath, datatype_class_canmemcpy>(BLIK_DBG 1);
        NewPath->Init(m_lastpos, NewPos);
        m_pathes.Enqueue((buffer) NewPath);
        m_lastpos = NewPos;
    }

    bool Tween2D::UpdateForRender()
    {
        if(!m_curpath && !(m_curpath = m_pathes.Dequeue()))
            return false;
        Tween2DPath* CurPath = (Tween2DPath*) m_curpath;
        m_curpos = CurPath->NextStep(m_curpath);
        return true;
    }

    bool Tween2D::UpdateForTick()
    {
        BLIK_ASSERT("차후 수정!!!", false);
        return false;
    }
}
