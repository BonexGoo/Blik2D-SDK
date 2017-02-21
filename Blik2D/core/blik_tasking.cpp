#include <blik.hpp>
#include "blik_tasking.hpp"

#include <platform/blik_platform.hpp>

class CommonClass
{
public:
    buffer Lock()
    {
        Mutex::Lock(m_mutex);
        return m_buffer;
    }

    void Unlock(buffer buf)
    {
        if(m_buffer != buf)
        {
            Buffer::Free(m_buffer);
            m_buffer = buf;
        }
        Mutex::Unlock(m_mutex);
    }

public:
    CommonClass()
    {
        m_buffer = nullptr;
        m_mutex = Mutex::Open();
    }

    ~CommonClass()
    {
        Buffer::Free(m_buffer);
        Mutex::Close(m_mutex);
    }

private:
    buffer m_buffer;
    id_mutex m_mutex;
};

class TaskingClass
{
public:
    enum BindingState {BS_Both, BS_OnlyTask, BS_OnlyUser, BS_WaitForTask};

public:
    bool IsAlive()
    {
        Mutex::Lock(m_mutex);
        bool Result = m_alived;
        Mutex::Unlock(m_mutex);
        return Result;
    }

    bool IsPause()
    {
        Mutex::Lock(m_mutex);
        bool Result = m_paused;
        Mutex::Unlock(m_mutex);
        return Result;
    }

    BindingState GetState()
    {
        Mutex::Lock(m_mutex);
        BindingState Result = m_state;
        Mutex::Unlock(m_mutex);
        return Result;
    }

    void DoDie()
    {
        Mutex::Lock(m_mutex);
        m_alived = false;
        AliveCounter().Set(false);
        Mutex::Unlock(m_mutex);
    }

    void SetPause(bool on)
    {
        Mutex::Lock(m_mutex);
        m_paused = on;
        Mutex::Unlock(m_mutex);
    }

    bool SetStateByCheck(BindingState check, BindingState state)
    {
        Mutex::Lock(m_mutex);
        bool Result = (m_state == check);
        if(!Result) m_state = state;
        Mutex::Unlock(m_mutex);
        return Result;
    }

public:
    static sint32 GetAliveCount()
    {
        return AliveCounter().Get();
    }

public:
    TaskingClass()
    {
        m_cb = nullptr;
        m_self = nullptr;
        m_alived = true;
        m_paused = false;
        m_state = BS_Both;
        m_mutex = Mutex::Open();
        AliveCounter().Set(true);
    }

    ~TaskingClass()
    {
        Buffer::Free(m_self);
        while(buffer RemBuffer = m_query.Dequeue())
            Buffer::Free(RemBuffer);
        while(buffer RemBuffer = m_answer.Dequeue())
            Buffer::Free(RemBuffer);
        Mutex::Close(m_mutex);
    }

public:
    Tasking::TaskCB m_cb;
    buffer m_self;
    Queue<buffer> m_query;
    Queue<buffer> m_answer;
    CommonClass m_common;

private:
    bool m_alived;
    bool m_paused;
    BindingState m_state;
    id_mutex m_mutex;

private:
    static SafeCounter& AliveCounter() {static SafeCounter _; return _;}
};

static void _TaskCore(void* arg)
{
    TaskingClass* This = (TaskingClass*) arg;

    sint32 NextSleep = 0;
    while(0 <= NextSleep && This->GetState() == TaskingClass::BS_Both)
    {
        if(This->IsPause())
        {
            Platform::Utility::Sleep(100, false);
            continue;
        }
        else Platform::Utility::Sleep(NextSleep, false);
        NextSleep = This->m_cb(This->m_self, This->m_query, This->m_answer, (id_common) &This->m_common);
    }

    This->DoDie();
    if(This->SetStateByCheck(TaskingClass::BS_OnlyTask, TaskingClass::BS_OnlyUser))
        Buffer::Free((buffer) This);
}

namespace BLIK
{
    id_tasking Tasking::Create(TaskCB cb, buffer self)
    {
        TaskingClass* NewTasking = (TaskingClass*) Buffer::Alloc<TaskingClass>(BLIK_DBG 1);
        NewTasking->m_cb = cb;
        NewTasking->m_self = self;
		Platform::Utility::Threading(_TaskCore, NewTasking);
        return (id_tasking) NewTasking;
    }

    void Tasking::Release(id_tasking tasking, bool doWait)
    {
        BLIK_ASSERT("tasking인수가 nullptr입니다", tasking);
        if(((TaskingClass*) tasking)->SetStateByCheck(TaskingClass::BS_OnlyUser,
            (doWait)? TaskingClass::BS_WaitForTask : TaskingClass::BS_OnlyTask))
            Buffer::Free((buffer) tasking);
        else if(doWait)
        {
            while(((TaskingClass*) tasking)->IsAlive())
                Platform::Utility::Sleep(10, false);
            Buffer::Free((buffer) tasking);
        }
    }

    void Tasking::Pause(id_tasking tasking)
    {
        BLIK_ASSERT("tasking인수가 nullptr입니다", tasking);
        ((TaskingClass*) tasking)->SetPause(true);
    }

    void Tasking::Resume(id_tasking tasking)
    {
        BLIK_ASSERT("tasking인수가 nullptr입니다", tasking);
        ((TaskingClass*) tasking)->SetPause(false);
    }

    bool Tasking::IsAlive(id_tasking tasking)
    {
        BLIK_ASSERT("tasking인수가 nullptr입니다", tasking);
        return ((TaskingClass*) tasking)->IsAlive();
    }

    sint32 Tasking::GetAliveCount()
    {
        return TaskingClass::GetAliveCount();
    }

    void Tasking::SendQuery(id_tasking tasking, buffer query)
    {
        BLIK_ASSERT("tasking인수가 nullptr입니다", tasking);
        ((TaskingClass*) tasking)->m_query.Enqueue(query);
    }

    buffer Tasking::GetAnswer(id_tasking tasking)
    {
        BLIK_ASSERT("tasking인수가 nullptr입니다", tasking);
        return ((TaskingClass*) tasking)->m_answer.Dequeue();
    }

    sint32 Tasking::GetAnswerCount(id_tasking tasking)
    {
        BLIK_ASSERT("tasking인수가 nullptr입니다", tasking);
        return ((TaskingClass*) tasking)->m_answer.Count();
    }

	void Tasking::KeepAnswer(id_tasking tasking, buffer answer)
    {
        BLIK_ASSERT("tasking인수가 nullptr입니다", tasking);
        ((TaskingClass*) tasking)->m_answer.Enqueue(answer);
    }

    buffer Tasking::LockCommon(id_tasking tasking, bool autounlock)
    {
        BLIK_ASSERT("tasking인수가 nullptr입니다", tasking);
        buffer Result = ((TaskingClass*) tasking)->m_common.Lock();
        if(autounlock && !Result)
            ((TaskingClass*) tasking)->m_common.Unlock(nullptr);
        return Result;
    }

    nullbuffer Tasking::UnlockCommon(id_tasking tasking, buffer buf)
    {
        BLIK_ASSERT("tasking인수가 nullptr입니다", tasking);
        ((TaskingClass*) tasking)->m_common.Unlock(buf);
        return nullptr;
    }

    buffer Tasking::LockCommonForTask(id_common common, bool autounlock)
    {
        BLIK_ASSERT("common인수가 nullptr입니다", common);
        buffer Result = ((CommonClass*) common)->Lock();
        if(autounlock && !Result)
            ((CommonClass*) common)->Unlock(nullptr);
        return Result;
    }

    nullbuffer Tasking::UnlockCommonForTask(id_common common, buffer buf)
    {
        BLIK_ASSERT("common인수가 nullptr입니다", common);
        ((CommonClass*) common)->Unlock(buf);
        return nullptr;
    }
}
