#include <blik.hpp>
#include "blik_kiwiservice.hpp"

#include <platform/blik_platform.hpp>

namespace BLIK
{
    KiwiService::KiwiService()
    {
        m_taskAnswer = nullptr;
        m_remote = nullptr;
        m_port = 0;
        m_addIcon_B2D = nullptr;
        m_runScript_D2B = nullptr;
        m_exitScript_D2B = nullptr;
        m_kickScript_B2D = nullptr;
        m_addTrace_B2D = nullptr;
        m_assertBreak_B2D = nullptr;
        m_getApiClasses_D2B = nullptr;
        m_getApiClassDetail_D2B = nullptr;
        m_getApiMethods_D2B = nullptr;
        m_getApiMethodDetail_D2B = nullptr;
        m_beginEvent_B2D = nullptr;
        m_endEvent_D2B = nullptr;
        m_createApiClass_D2B = nullptr;
        m_removeApiClass_D2B = nullptr;
        m_callApiMethod_D2B = nullptr;
    }

    KiwiService::~KiwiService()
    {
        BLIK_ASSERT("잘못된 시나리오입니다", !m_addIcon_B2D);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_runScript_D2B);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_exitScript_D2B);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_kickScript_B2D);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_addTrace_B2D);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_assertBreak_B2D);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_getApiClasses_D2B);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_getApiClassDetail_D2B);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_getApiMethods_D2B);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_getApiMethodDetail_D2B);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_beginEvent_B2D);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_endEvent_D2B);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_createApiClass_D2B);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_removeApiClass_D2B);
        BLIK_ASSERT("잘못된 시나리오입니다", !m_callApiMethod_D2B);
    }

    KiwiService& KiwiService::operator=(const KiwiService&)
    {
        BLIK_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

    void KiwiService::SettingForServer(uint16 port)
    {
        m_domain = "";
        m_port = port;
    }

    void KiwiService::SettingForClient(chars domain, uint16 port)
    {
        m_domain = domain;
        m_port = port;
    }

    bool KiwiService::AddAnswer(buffer answer)
    {
        if(m_taskAnswer)
        {
            m_taskAnswer->Enqueue(answer);
            return true;
        }
        return false;
    }

    void KiwiService::InitForThread()
    {
        BLIK_ASSERT("잘못된 시나리오입니다", false);
    }

    void KiwiService::QuitForThread()
    {
        Remote::Disconnect(m_remote);
        m_remote = nullptr;

        delete m_addIcon_B2D;
        delete m_runScript_D2B;
        delete m_exitScript_D2B;
        delete m_kickScript_B2D;
        delete m_addTrace_B2D;
        delete m_assertBreak_B2D;
        delete m_getApiClasses_D2B;
        delete m_getApiClassDetail_D2B;
        delete m_getApiMethods_D2B;
        delete m_getApiMethodDetail_D2B;
        delete m_beginEvent_B2D;
        delete m_endEvent_D2B;
        delete m_createApiClass_D2B;
        delete m_removeApiClass_D2B;
        delete m_callApiMethod_D2B;
        m_addIcon_B2D = nullptr;
        m_runScript_D2B = nullptr;
        m_exitScript_D2B = nullptr;
        m_kickScript_B2D = nullptr;
        m_addTrace_B2D = nullptr;
        m_assertBreak_B2D = nullptr;
        m_getApiClasses_D2B = nullptr;
        m_getApiClassDetail_D2B = nullptr;
        m_getApiMethods_D2B = nullptr;
        m_getApiMethodDetail_D2B = nullptr;
        m_beginEvent_B2D = nullptr;
        m_endEvent_D2B = nullptr;
        m_createApiClass_D2B = nullptr;
        m_removeApiClass_D2B = nullptr;
        m_callApiMethod_D2B = nullptr;
    }

    void KiwiService::BindTaskQueue(Queue<buffer>* taskAnswer)
    {
        m_taskAnswer = taskAnswer;
    }

    void KiwiService::UnbindTaskQueue()
    {
        m_taskAnswer = nullptr;
    }

    void KiwiService::LinkStep_AddIcon_BotToDebugger(chars name, bytes icon, sint32 iconLength) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_addIcon_B2D);
        (*m_addIcon_B2D)(name)(icon, iconLength);
    }

    void KiwiService::LinkStep_RunScript_DebuggerToBot(chars mainClassName, sint32 runID) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_runScript_D2B);
        (*m_runScript_D2B)(mainClassName)(runID);
    }

    void KiwiService::LinkStep_ExitScript_DebuggerToBot(sint32 runID) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_exitScript_D2B);
        (*m_exitScript_D2B)(runID);
    }

    void KiwiService::LinkStep_KickScript_BotToDebugger(sint32 runID) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_kickScript_B2D);
        (*m_kickScript_B2D)(runID);
    }

    void KiwiService::DebugStep_AddTrace_BotToDebugger(sint32 level, chars comment) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_addTrace_B2D);
        (*m_addTrace_B2D)(level)(comment);
    }

    sint32 KiwiService::DebugStep_AssertBreak_BotToDebugger(chars comment, chars filename, sint32 linenumber, chars funcname) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_assertBreak_B2D);
        Remote::IntParam Result((id_cloned_share) (*m_assertBreak_B2D)(comment)(filename)(linenumber)(funcname));
        return (sint32) Result.ConstValue();
    }

    String KiwiService::DebugStep_GetApiClasses_DebuggerToBot() const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_getApiClasses_D2B);
        return (id_cloned_share) (*m_getApiClasses_D2B)();
    }

    String KiwiService::DebugStep_GetApiClassDetail_DebuggerToBot(chars className) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_getApiClassDetail_D2B);
        return (id_cloned_share) (*m_getApiClassDetail_D2B)(className);
    }

    String KiwiService::DebugStep_GetApiMethods_DebuggerToBot(chars className) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_getApiMethods_D2B);
        return (id_cloned_share) (*m_getApiMethods_D2B)(className);
    }

    String KiwiService::DebugStep_GetApiMethodDetail_DebuggerToBot(chars className, chars methodName) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_getApiMethodDetail_D2B);
        return (id_cloned_share) (*m_getApiMethodDetail_D2B)(className)(methodName);
    }

    const Remote::Method KiwiService::RunStep_BeginEvent_BotToDebugger(chars methodName, sint32 eventID) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_beginEvent_B2D);
        return (*m_beginEvent_B2D)(methodName)(eventID);
    }

    void KiwiService::RunStep_EndEvent_DebuggerToBot(sint32 eventID, sint32 success) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_endEvent_D2B);
        (*m_endEvent_D2B)(eventID)(success);
    }

    void KiwiService::RunStep_CreateApiClass_DebuggerToBot(chars className, sint32 classID) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_createApiClass_D2B);
        (*m_createApiClass_D2B)(className)(classID);
    }

    void KiwiService::RunStep_RemoveApiClass_DebuggerToBot(sint32 classID) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_removeApiClass_D2B);
        (*m_removeApiClass_D2B)(classID);
    }

    const Remote::Method KiwiService::RunStep_CallApiMethod_DebuggerToBot(sint32 classID, chars methodName) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_callApiMethod_D2B);
        return (*m_callApiMethod_D2B)(classID)(methodName);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // KiwiDebuggerService
    ////////////////////////////////////////////////////////////////////////////////
    KiwiDebuggerService::KiwiDebuggerService()
    {
        m_isRunning = false;
        m_lastRunID = -1;
    }

    KiwiDebuggerService::~KiwiDebuggerService()
    {
    }

    KiwiDebuggerService& KiwiDebuggerService::operator=(const KiwiDebuggerService&)
    {
        BLIK_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

    void KiwiDebuggerService::InitForThread()
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_remote == nullptr);
        m_addIcon_B2D = new Remote::Method("OnAddIcon", OnAddIcon, this);
        m_runScript_D2B = new Remote::Method("OnRunScript", OnNull, this);
        m_exitScript_D2B = new Remote::Method("OnExitScript", OnNull, this);
        m_kickScript_B2D = new Remote::Method("OnKickScript", OnKickScript, this);
        m_addTrace_B2D = new Remote::Method("OnAddTrace", OnAddTrace, this);
        m_assertBreak_B2D = new Remote::Method("OnAssertBreak", OnAssertBreak, this);
        m_getApiClasses_D2B = new Remote::Method("OnGetApiClasses", OnNull, this);
        m_getApiClassDetail_D2B = new Remote::Method("OnGetApiClassDetail", OnNull, this);
        m_getApiMethods_D2B = new Remote::Method("OnGetApiMethods", OnNull, this);
        m_getApiMethodDetail_D2B = new Remote::Method("OnGetApiMethodDetail", OnNull, this);
        m_beginEvent_B2D = new Remote::Method("OnBeginEvent", OnBeginEvent, this);
        m_endEvent_D2B = new Remote::Method("OnEndEvent", OnNull, this);
        m_createApiClass_D2B = new Remote::Method("OnCreateApiClass", OnNull, this);
        m_removeApiClass_D2B = new Remote::Method("OnRemoveApiClass", OnNull, this);
        m_callApiMethod_D2B = new Remote::Method("OnCallApiMethod", OnNull, this);

        BLIK_ASSERT("접속정보가 없습니다", m_port != 0);
        Remote::MethodPtrs Methods;
        Methods.AtAdding() = m_addIcon_B2D;
        Methods.AtAdding() = m_runScript_D2B;
        Methods.AtAdding() = m_exitScript_D2B;
        Methods.AtAdding() = m_kickScript_B2D;
        Methods.AtAdding() = m_addTrace_B2D;
        Methods.AtAdding() = m_assertBreak_B2D;
        Methods.AtAdding() = m_getApiClasses_D2B;
        Methods.AtAdding() = m_getApiClassDetail_D2B;
        Methods.AtAdding() = m_getApiMethods_D2B;
        Methods.AtAdding() = m_getApiMethodDetail_D2B;
        Methods.AtAdding() = m_beginEvent_B2D;
        Methods.AtAdding() = m_endEvent_D2B;
        Methods.AtAdding() = m_createApiClass_D2B;
        Methods.AtAdding() = m_removeApiClass_D2B;
        Methods.AtAdding() = m_callApiMethod_D2B;
        if(0 < m_domain.Length())
            m_remote = Remote::ConnectForClient(m_domain, m_port, Methods);
        else m_remote = Remote::ConnectForServer(m_port, Methods);
    }

    bool KiwiDebuggerService::IsRunning()
    {
        return m_isRunning;
    }

    buffer KiwiDebuggerService::RunScript(chars mainClassName)
    {
        if(!m_isRunning)
        {
            const sint32 NewRunID = ++m_lastRunID;
            m_isRunning = true;
            LinkStep_RunScript_DebuggerToBot(mainClassName, NewRunID);

            auto NewInitClass = (KiwiEvent::InitClass*) Buffer::Alloc<KiwiEvent::InitClass>(BLIK_DBG 1);
            NewInitClass->m_runID = NewRunID;
            return (buffer) NewInitClass;
        }
        return nullptr;
    }

    buffer KiwiDebuggerService::ExitScript()
    {
        if(m_isRunning)
        {
            m_isRunning = false;
            LinkStep_ExitScript_DebuggerToBot(m_lastRunID);

            auto NewQuitClass = (KiwiEvent::QuitClass*) Buffer::Alloc<KiwiEvent::QuitClass>(BLIK_DBG 1);
            return (buffer) NewQuitClass;
        }
        return nullptr;
    }

    const Remote::Method KiwiDebuggerService::CallApiMethod(sint32 classID, chars methodName)
    {
        return RunStep_CallApiMethod_DebuggerToBot(classID, methodName);
    }

    void KiwiDebuggerService::EndEvent(sint32 eventID, bool success)
    {
        if(m_isRunning)
            RunStep_EndEvent_DebuggerToBot(eventID, (sint32) success);
    }

    void KiwiDebuggerService::OnAddIcon(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiDebuggerService* Self = (KiwiDebuggerService*) data;

        auto NewIconPack = (KiwiEvent::IconPack*) Buffer::Alloc<KiwiEvent::IconPack>(BLIK_DBG 1);
        NewIconPack->m_botName = params[0].Drain();
        NewIconPack->m_iconBinary = params[1].Drain();

        const bool Result = Self->AddAnswer((buffer) NewIconPack);
        BLIK_ASSERT("잘못된 시나리오입니다", Result);
    }

    void KiwiDebuggerService::OnKickScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiDebuggerService* Self = (KiwiDebuggerService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void KiwiDebuggerService::OnAddTrace(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiDebuggerService* Self = (KiwiDebuggerService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void KiwiDebuggerService::OnAssertBreak(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiDebuggerService* Self = (KiwiDebuggerService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void KiwiDebuggerService::OnBeginEvent(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiDebuggerService* Self = (KiwiDebuggerService*) data;

        auto NewCallEvent = (KiwiEvent::CallEvent*) Buffer::Alloc<KiwiEvent::CallEvent>(BLIK_DBG 1);
        NewCallEvent->m_methodName = params[0].Drain();
        NewCallEvent->m_eventID = (sint32) Remote::IntParam(params[1].Drain()).ConstValue();
        for(sint32 i = 2, iend = params.Count(); i < iend; ++i)
            NewCallEvent->m_params.AtAdding().Store(params[i].Drain());

        const bool Result = Self->AddAnswer((buffer) NewCallEvent);
        BLIK_ASSERT("잘못된 시나리오입니다", Result);
    }

    void KiwiDebuggerService::OnNull(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("봇이 시나리오에 맞지 않는 잘못된 호출을 하였습니다", localcall);
    }

    sint32 KiwiDebuggerService::OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        KiwiDebuggerService* Self = (KiwiDebuggerService*) self;
        Self->BindTaskQueue(&answer);
        if(!Self->m_remote) Self->InitForThread();
        const bool IsConnected = Remote::CommunicateOnce(Self->m_remote);
        Self->UnbindTaskQueue();

        // 어플리케이션과의 통신
        while(buffer OneBuffer = query.Dequeue())
        {
            chars Text = (chars) OneBuffer;
            if(!String::Compare(Text, "{exit}"))
            {
                Self->QuitForThread();
                Buffer::Free(OneBuffer);
                return -1;
            }
            Buffer::Free(OneBuffer);
        }
        return 100;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // KiwiBotService
    ////////////////////////////////////////////////////////////////////////////////
    KiwiBotService::KiwiBotService()
    {
        m_needSighUp = true;
        m_isRunning = false;
        m_isEventing = false;
        m_lastEventID = -1;
        m_icon = nullptr;
    }

    KiwiBotService::~KiwiBotService()
    {
        Buffer::Free(m_icon);
    }

    KiwiBotService& KiwiBotService::operator=(const KiwiBotService&)
    {
        BLIK_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

    void KiwiBotService::SetIcon(chars name, chars filename)
    {
        m_name = name;

        id_asset_read File = Asset::OpenForRead(filename);
        sint32 FileSize = Asset::Size(File);
        Buffer::Free(m_icon);

        m_icon = Buffer::Alloc(BLIK_DBG FileSize);
        Asset::Read(File, (uint08*) m_icon, FileSize);
        Asset::Close(File);
    }

    bool KiwiBotService::IsRunningAndNotEventing()
    {
        return (m_isRunning && !m_isEventing);
    }

    void KiwiBotService::AddApiMethod(chars apiMethodName, Remote::Method::GlueSubCB cb)
    {
        m_apiMethod(apiMethodName) = cb;
    }

    const Remote::Method KiwiBotService::BeginEvent(chars methodName)
    {
        if(m_isRunning && !m_isEventing)
        {
            m_isEventing = true;
            return RunStep_BeginEvent_BotToDebugger(methodName, ++m_lastEventID);
        }
        return Remote::Method();
    }

    void KiwiBotService::EndEventProc(EndEventResult result)
    {
        if(m_isEventing)
        {
            m_isEventing = false;

            /////////////////////////////////////////////////////
            /////////////////////////////////////////////////////
            /////////////////////////////////////////////////////

            bool Result = false;
            switch(result)
            {
            case EER_Done:
                Result = AddAnswer(Buffer::Alloc<KiwiEvent::EventResult_Done>(BLIK_DBG 1));
                break;
            case EER_ExitScript:
                Result = AddAnswer(Buffer::Alloc<KiwiEvent::EventResult_ExitScript>(BLIK_DBG 1));
                break;
            case EER_NotFound:
                Result = AddAnswer(Buffer::Alloc<KiwiEvent::EventResult_NotFound>(BLIK_DBG 1));
                break;
            }
            BLIK_ASSERT("잘못된 시나리오입니다", Result);
        }
    }

    void KiwiBotService::InitForThread()
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_remote == nullptr);
        m_addIcon_B2D = new Remote::Method("OnAddIcon", OnNull, this);
        m_runScript_D2B = new Remote::Method("OnRunScript", OnRunScript, this);
        m_exitScript_D2B = new Remote::Method("OnExitScript", OnExitScript, this);
        m_kickScript_B2D = new Remote::Method("OnKickScript", OnNull, this);
        m_addTrace_B2D = new Remote::Method("OnAddTrace", OnNull, this);
        m_assertBreak_B2D = new Remote::Method("OnAssertBreak", OnNull, this);
        m_getApiClasses_D2B = new Remote::Method("OnGetApiClasses", OnGetApiClasses, this);
        m_getApiClassDetail_D2B = new Remote::Method("OnGetApiClassDetail", OnGetApiClassDetail, this);
        m_getApiMethods_D2B = new Remote::Method("OnGetApiMethods", OnGetApiMethods, this);
        m_getApiMethodDetail_D2B = new Remote::Method("OnGetApiMethodDetail", OnGetApiMethodDetail, this);
        m_beginEvent_B2D = new Remote::Method("OnBeginEvent", OnNull, this);
        m_endEvent_D2B = new Remote::Method("OnEndEvent", OnEndEvent, this);
        m_createApiClass_D2B = new Remote::Method("OnCreateApiClass", OnCreateApiClass, this);
        m_removeApiClass_D2B = new Remote::Method("OnRemoveApiClass", OnRemoveApiClass, this);
        m_callApiMethod_D2B = new Remote::Method("OnCallApiMethod", OnCallApiMethod, this);

        BLIK_ASSERT("접속정보가 없습니다", m_port != 0);
        Remote::MethodPtrs Methods;
        Methods.AtAdding() = m_addIcon_B2D;
        Methods.AtAdding() = m_runScript_D2B;
        Methods.AtAdding() = m_exitScript_D2B;
        Methods.AtAdding() = m_kickScript_B2D;
        Methods.AtAdding() = m_addTrace_B2D;
        Methods.AtAdding() = m_assertBreak_B2D;
        Methods.AtAdding() = m_getApiClasses_D2B;
        Methods.AtAdding() = m_getApiClassDetail_D2B;
        Methods.AtAdding() = m_getApiMethods_D2B;
        Methods.AtAdding() = m_getApiMethodDetail_D2B;
        Methods.AtAdding() = m_beginEvent_B2D;
        Methods.AtAdding() = m_endEvent_D2B;
        Methods.AtAdding() = m_createApiClass_D2B;
        Methods.AtAdding() = m_removeApiClass_D2B;
        Methods.AtAdding() = m_callApiMethod_D2B;
        if(0 < m_domain.Length())
            m_remote = Remote::ConnectForClient(m_domain, m_port, Methods);
        else m_remote = Remote::ConnectForServer(m_port, Methods);
    }

    void KiwiBotService::OnRunScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiBotService* Self = (KiwiBotService*) data;

        Self->m_isRunning = true;
        Self->AddAnswer(Buffer::Alloc<KiwiEvent::RunScript>(BLIK_DBG 1));
    }

    void KiwiBotService::OnExitScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiBotService* Self = (KiwiBotService*) data;

        Self->m_isRunning = false;
        Self->EndEventProc(EER_ExitScript);
    }

    void KiwiBotService::OnGetApiClasses(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiBotService* Self = (KiwiBotService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void KiwiBotService::OnGetApiClassDetail(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiBotService* Self = (KiwiBotService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void KiwiBotService::OnGetApiMethods(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiBotService* Self = (KiwiBotService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void KiwiBotService::OnGetApiMethodDetail(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiBotService* Self = (KiwiBotService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void KiwiBotService::OnEndEvent(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiBotService* Self = (KiwiBotService*) data;

        Remote::IntParam EventID = params[0].Drain();
        Remote::IntParam Success = params[1].Drain();
        Self->EndEventProc((Success.ConstValue())? EER_Done : EER_NotFound);
    }

    void KiwiBotService::OnCreateApiClass(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiBotService* Self = (KiwiBotService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void KiwiBotService::OnRemoveApiClass(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiBotService* Self = (KiwiBotService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void KiwiBotService::OnCallApiMethod(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        KiwiBotService* Self = (KiwiBotService*) data;

        Remote::IntParam ClassID = params[0].Drain();
        Remote::StrParam MethodName = params[1].Drain();
        if(auto CurMethod = Self->m_apiMethod.Access(MethodName))
            (*CurMethod)(data, &params[2], params.Count() - 2, ret);
    }

    void KiwiBotService::OnNull(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("디버거가 시나리오에 맞지 않는 잘못된 호출을 하였습니다", localcall);
    }

    sint32 KiwiBotService::OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        KiwiBotService* Self = (KiwiBotService*) self;
        Self->BindTaskQueue(&answer);
        if(!Self->m_remote) Self->InitForThread();
        const bool IsConnected = Remote::CommunicateOnce(Self->m_remote);
        Self->UnbindTaskQueue();

        if(IsConnected && Self->m_needSighUp)
        {
            Self->m_needSighUp = false;
            Self->LinkStep_AddIcon_BotToDebugger(Self->m_name, (bytes) Self->m_icon, Buffer::CountOf(Self->m_icon));
        }

        // 어플리케이션과의 통신
        while(buffer OneBuffer = query.Dequeue())
        {
            chars Text = (chars) OneBuffer;
            if(!String::Compare(Text, "{exit}"))
            {
                Self->QuitForThread();
                Buffer::Free(OneBuffer);
                return -1;
            }
            Buffer::Free(OneBuffer);
        }
        return 100;
    }
}
