#include <blik.hpp>
#include "blik_briiservice.hpp"

#include <platform/blik_platform.hpp>

namespace BLIK
{
    BriiService::BriiService()
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

    BriiService::~BriiService()
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

    BriiService& BriiService::operator=(const BriiService&)
    {
        BLIK_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

    void BriiService::SettingForServer(uint16 port)
    {
        m_domain = "";
        m_port = port;
    }

    void BriiService::SettingForClient(chars domain, uint16 port)
    {
        m_domain = domain;
        m_port = port;
    }

    bool BriiService::AddAnswer(buffer answer)
    {
        if(m_taskAnswer)
        {
            m_taskAnswer->Enqueue(answer);
            return true;
        }
        return false;
    }

    void BriiService::InitForThread()
    {
        BLIK_ASSERT("잘못된 시나리오입니다", false);
    }

    void BriiService::QuitForThread()
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

    void BriiService::BindTaskQueue(Queue<buffer>* taskAnswer)
    {
        m_taskAnswer = taskAnswer;
    }

    void BriiService::UnbindTaskQueue()
    {
        m_taskAnswer = nullptr;
    }

    void BriiService::LinkStep_AddIcon_BotToDebugger(chars name, bytes icon, sint32 iconLength) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_addIcon_B2D);
        (*m_addIcon_B2D)(name)(icon, iconLength);
    }

    void BriiService::LinkStep_RunScript_DebuggerToBot(chars mainClassName, sint32 runID) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_runScript_D2B);
        (*m_runScript_D2B)(mainClassName)(runID);
    }

    void BriiService::LinkStep_ExitScript_DebuggerToBot(sint32 runID) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_exitScript_D2B);
        (*m_exitScript_D2B)(runID);
    }

    void BriiService::LinkStep_KickScript_BotToDebugger(sint32 runID) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_kickScript_B2D);
        (*m_kickScript_B2D)(runID);
    }

    void BriiService::DebugStep_AddTrace_BotToDebugger(sint32 level, chars comment) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_addTrace_B2D);
        (*m_addTrace_B2D)(level)(comment);
    }

    sint32 BriiService::DebugStep_AssertBreak_BotToDebugger(chars comment, chars filename, sint32 linenumber, chars funcname) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_assertBreak_B2D);
        Remote::IntParam Result((id_cloned_share) (*m_assertBreak_B2D)(comment)(filename)(linenumber)(funcname));
        return (sint32) Result.ConstValue();
    }

    String BriiService::DebugStep_GetApiClasses_DebuggerToBot() const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_getApiClasses_D2B);
        return (id_cloned_share) (*m_getApiClasses_D2B)();
    }

    String BriiService::DebugStep_GetApiClassDetail_DebuggerToBot(chars className) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_getApiClassDetail_D2B);
        return (id_cloned_share) (*m_getApiClassDetail_D2B)(className);
    }

    String BriiService::DebugStep_GetApiMethods_DebuggerToBot(chars className) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_getApiMethods_D2B);
        return (id_cloned_share) (*m_getApiMethods_D2B)(className);
    }

    String BriiService::DebugStep_GetApiMethodDetail_DebuggerToBot(chars className, chars methodName) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_getApiMethodDetail_D2B);
        return (id_cloned_share) (*m_getApiMethodDetail_D2B)(className)(methodName);
    }

    const Remote::Method BriiService::RunStep_BeginEvent_BotToDebugger(chars methodName, sint32 eventID) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_beginEvent_B2D);
        return (*m_beginEvent_B2D)(methodName)(eventID);
    }

    void BriiService::RunStep_EndEvent_DebuggerToBot(sint32 eventID, sint32 success) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_endEvent_D2B);
        (*m_endEvent_D2B)(eventID)(success);
    }

    void BriiService::RunStep_CreateApiClass_DebuggerToBot(chars className, sint32 classID) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_createApiClass_D2B);
        (*m_createApiClass_D2B)(className)(classID);
    }

    void BriiService::RunStep_RemoveApiClass_DebuggerToBot(sint32 classID) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_removeApiClass_D2B);
        (*m_removeApiClass_D2B)(classID);
    }

    const Remote::Method BriiService::RunStep_CallApiMethod_DebuggerToBot(sint32 classID, chars methodName) const
    {
        BLIK_ASSERT("잘못된 시나리오입니다", m_callApiMethod_D2B);
        return (*m_callApiMethod_D2B)(classID)(methodName);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // BriiDebuggerService
    ////////////////////////////////////////////////////////////////////////////////
    BriiDebuggerService::BriiDebuggerService()
    {
        m_isRunning = false;
        m_lastRunID = -1;
    }

    BriiDebuggerService::~BriiDebuggerService()
    {
    }

    BriiDebuggerService& BriiDebuggerService::operator=(const BriiDebuggerService&)
    {
        BLIK_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

    void BriiDebuggerService::InitForThread()
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

    bool BriiDebuggerService::IsRunning()
    {
        return m_isRunning;
    }

    buffer BriiDebuggerService::RunScript(chars mainClassName)
    {
        if(!m_isRunning)
        {
            const sint32 NewRunID = ++m_lastRunID;
            m_isRunning = true;
            LinkStep_RunScript_DebuggerToBot(mainClassName, NewRunID);

            auto NewInitClass = (BriiEvent::InitClass*) Buffer::Alloc<BriiEvent::InitClass>(BLIK_DBG 1);
            NewInitClass->m_runID = NewRunID;
            return (buffer) NewInitClass;
        }
        return nullptr;
    }

    buffer BriiDebuggerService::ExitScript()
    {
        if(m_isRunning)
        {
            m_isRunning = false;
            LinkStep_ExitScript_DebuggerToBot(m_lastRunID);

            auto NewQuitClass = (BriiEvent::QuitClass*) Buffer::Alloc<BriiEvent::QuitClass>(BLIK_DBG 1);
            return (buffer) NewQuitClass;
        }
        return nullptr;
    }

    const Remote::Method BriiDebuggerService::CallApiMethod(sint32 classID, chars methodName)
    {
        return RunStep_CallApiMethod_DebuggerToBot(classID, methodName);
    }

    void BriiDebuggerService::EndEvent(sint32 eventID, bool success)
    {
        if(m_isRunning)
            RunStep_EndEvent_DebuggerToBot(eventID, (sint32) success);
    }

    void BriiDebuggerService::OnAddIcon(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiDebuggerService* Self = (BriiDebuggerService*) data;

        auto NewIconPack = (BriiEvent::IconPack*) Buffer::Alloc<BriiEvent::IconPack>(BLIK_DBG 1);
        NewIconPack->m_botName = params[0].Drain();
        NewIconPack->m_iconBinary = params[1].Drain();

        const bool Result = Self->AddAnswer((buffer) NewIconPack);
        BLIK_ASSERT("잘못된 시나리오입니다", Result);
    }

    void BriiDebuggerService::OnKickScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiDebuggerService* Self = (BriiDebuggerService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiDebuggerService::OnAddTrace(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiDebuggerService* Self = (BriiDebuggerService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiDebuggerService::OnAssertBreak(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiDebuggerService* Self = (BriiDebuggerService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiDebuggerService::OnBeginEvent(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiDebuggerService* Self = (BriiDebuggerService*) data;

        auto NewCallEvent = (BriiEvent::CallEvent*) Buffer::Alloc<BriiEvent::CallEvent>(BLIK_DBG 1);
        NewCallEvent->m_methodName = params[0].Drain();
        NewCallEvent->m_eventID = (sint32) Remote::IntParam(params[1].Drain()).ConstValue();
        for(sint32 i = 2, iend = params.Count(); i < iend; ++i)
            NewCallEvent->m_params.AtAdding().Store(params[i].Drain());

        const bool Result = Self->AddAnswer((buffer) NewCallEvent);
        BLIK_ASSERT("잘못된 시나리오입니다", Result);
    }

    void BriiDebuggerService::OnNull(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("봇이 시나리오에 맞지 않는 잘못된 호출을 하였습니다", localcall);
    }

    sint32 BriiDebuggerService::OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        BriiDebuggerService* Self = (BriiDebuggerService*) self;
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
    // BriiBotService
    ////////////////////////////////////////////////////////////////////////////////
    BriiBotService::BriiBotService()
    {
        m_needSighUp = true;
        m_isRunning = false;
        m_isEventing = false;
        m_lastEventID = -1;
        m_icon = nullptr;
    }

    BriiBotService::~BriiBotService()
    {
        Buffer::Free(m_icon);
    }

    BriiBotService& BriiBotService::operator=(const BriiBotService&)
    {
        BLIK_ASSERT("해당 객체는 복사할 수 없습니다", false);
        return *this;
    }

    void BriiBotService::SetIcon(chars name, chars filename)
    {
        m_name = name;

        id_asset_read File = Asset::OpenForRead(filename);
        sint32 FileSize = Asset::Size(File);
        Buffer::Free(m_icon);

        m_icon = Buffer::Alloc(BLIK_DBG FileSize);
        Asset::Read(File, (uint08*) m_icon, FileSize);
        Asset::Close(File);
    }

    bool BriiBotService::IsRunningAndNotEventing()
    {
        return (m_isRunning && !m_isEventing);
    }

    void BriiBotService::AddApiMethod(chars apiMethodName, Remote::Method::GlueSubCB cb)
    {
        m_apiMethod(apiMethodName) = cb;
    }

    const Remote::Method BriiBotService::BeginEvent(chars methodName)
    {
        if(m_isRunning && !m_isEventing)
        {
            m_isEventing = true;
            return RunStep_BeginEvent_BotToDebugger(methodName, ++m_lastEventID);
        }
        return Remote::Method();
    }

    void BriiBotService::EndEventProc(EndEventResult result)
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
                Result = AddAnswer(Buffer::Alloc<BriiEvent::EventResult_Done>(BLIK_DBG 1));
                break;
            case EER_ExitScript:
                Result = AddAnswer(Buffer::Alloc<BriiEvent::EventResult_ExitScript>(BLIK_DBG 1));
                break;
            case EER_NotFound:
                Result = AddAnswer(Buffer::Alloc<BriiEvent::EventResult_NotFound>(BLIK_DBG 1));
                break;
            }
            BLIK_ASSERT("잘못된 시나리오입니다", Result);
        }
    }

    void BriiBotService::InitForThread()
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

    void BriiBotService::OnRunScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiBotService* Self = (BriiBotService*) data;

        Self->m_isRunning = true;
        Self->AddAnswer(Buffer::Alloc<BriiEvent::RunScript>(BLIK_DBG 1));
    }

    void BriiBotService::OnExitScript(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiBotService* Self = (BriiBotService*) data;

        Self->m_isRunning = false;
        Self->EndEventProc(EER_ExitScript);
    }

    void BriiBotService::OnGetApiClasses(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiBotService* Self = (BriiBotService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiBotService::OnGetApiClassDetail(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiBotService* Self = (BriiBotService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiBotService::OnGetApiMethods(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiBotService* Self = (BriiBotService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiBotService::OnGetApiMethodDetail(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiBotService* Self = (BriiBotService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiBotService::OnEndEvent(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiBotService* Self = (BriiBotService*) data;

        Remote::IntParam EventID = params[0].Drain();
        Remote::IntParam Success = params[1].Drain();
        Self->EndEventProc((Success.ConstValue())? EER_Done : EER_NotFound);
    }

    void BriiBotService::OnCreateApiClass(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiBotService* Self = (BriiBotService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiBotService::OnRemoveApiClass(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiBotService* Self = (BriiBotService*) data;

        BLIK_ASSERT("구현필요!!!", false); ///////////////////////
    }

    void BriiBotService::OnCallApiMethod(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("잘못된 시나리오입니다", data);
        BriiBotService* Self = (BriiBotService*) data;

        Remote::IntParam ClassID = params[0].Drain();
        Remote::StrParam MethodName = params[1].Drain();
        if(auto CurMethod = Self->m_apiMethod.Access(MethodName))
            (*CurMethod)(data, &params[2], params.Count() - 2, ret);
    }

    void BriiBotService::OnNull(payload data, const Remote::Params& params, id_cloned_share* ret, bool localcall)
    {
        BLIK_ASSERT("디버거가 시나리오에 맞지 않는 잘못된 호출을 하였습니다", localcall);
    }

    sint32 BriiBotService::OnTask(buffer& self, Queue<buffer>& query, Queue<buffer>& answer, id_common common)
    {
        BriiBotService* Self = (BriiBotService*) self;
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
