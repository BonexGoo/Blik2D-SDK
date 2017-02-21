#pragma once
#include <blik.hpp>

namespace BLIK
{
    //! \brief 빌더-배치
    class BatchBuilder
    {
    public:
        BatchBuilder(chars dirname);
        ~BatchBuilder();
        BatchBuilder& operator=(const BatchBuilder&)
        {
            BLIK_ASSERT("해당 객체는 복사할 수 없습니다", false);
            return *this;
        }

    public:
        void Processing(chars filename);

    protected:
        void Process_IncludeAlias(const String& command, sint32 pos, chars prm, chars comment);
        static void Process_IncludeAliasCB(chars itemname, payload data);
        void Build_IncludeAlias(chars sourcepath, const Strings& dsts, const Strings& srcs, chars comment);

    private:
        String ReadFile(chars filename);

    private:
        const String m_dirname;
        const Strings* m_ref_dsts;
        const Strings* m_ref_srcs;
        chars m_ref_comment;
    };
}
