#include <blik.hpp>
#include "blik_solver.hpp"

namespace BLIK
{
    // 상수항
    class Literal : public Operand
    {
        BLIK_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(Literal, mValue(rhs.mValue))
        public: Literal(SolverFloat value = 0) : Operand(OperandType::Literal), mValue(value) {}
        public: ~Literal() {}

        // 가상 인터페이스
        public: void PrintString(String& collector) const override
        {collector += String::FromFloat(mValue);}
        public: void UpdateChain(Solver* solver, SolverChain* chain) override {}
        public: float reliable() const override {return 1;}
        public: SolverFloat result(SolverFloat zero) const override {return mValue;}
        public: buffer clone() const override
        {
            buffer NewBuffer = Buffer::AllocNoConstructorOnce<Literal>(BLIK_DBG 1);
            BLIK_CONSTRUCTOR(NewBuffer, 0, Literal, mValue);
            return NewBuffer;
        }

        // 멤버
        public: const SolverFloat mValue; // 값
    };

    // 변수항
    class Variable : public Operand
    {
        BLIK_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(Variable, mName(rhs.mName))
        public: Variable(chars name = "") : Operand(OperandType::Variable), mName(name)
        {mSolver = nullptr; mChain = nullptr;}
        public: ~Variable() {RemoveCurrentChain();}

        // 비공개부
        private: void RemoveCurrentChain()
        {
            if(mSolver && mChain)
            {
                // 기존 체인을 제거
                if(auto CurChainPair = mChain->Access(mName))
                if(CurChainPair->SubObserver(mSolver))
                    mChain->Remove(mName);
                mSolver = nullptr;
                mChain = nullptr;
            }
        }

        // 가상 인터페이스
        public: void PrintString(String& collector) const override
        {collector += mName;}
        public: void UpdateChain(Solver* solver, SolverChain* chain) override
        {
            RemoveCurrentChain();
            // 신규 체인을 연결
            mSolver = solver;
            mChain = chain;
            if(mSolver && mChain)
                (*mChain)(mName).AddObserver(mSolver);
        }
        public: float reliable() const override
        {
            if(auto CurChainPair = mChain->Access(mName))
            if(auto CurSolver = CurChainPair->dest())
                return CurSolver->reliable();
            return 0;
        }
        public: SolverFloat result(SolverFloat zero) const override
        {
            if(0 < reliable())
            if(auto CurChainPair = mChain->Access(mName))
            if(auto CurSolver = CurChainPair->dest())
                return CurSolver->result();
            return zero;
        }
        public: buffer clone() const override
        {
            buffer NewBuffer = Buffer::AllocNoConstructorOnce<Variable>(BLIK_DBG 1);
            BLIK_CONSTRUCTOR(NewBuffer, 0, Variable, mName);
            ((Variable*) NewBuffer)->UpdateChain(mSolver, mChain);
            return NewBuffer;
        }

        // 멤버
        private: const String mName; // 이름
        private: Solver* mSolver; // 자기계산식
        private: SolverChain* mChain; // 검색체인
    };

    // 계산항
    class Formula : public Operand
    {
        BLIK_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(Formula, mOperatorType(OperatorType::Unknown), mOperatorPriority(0))
        public: Formula(OperatorType type = OperatorType::Unknown, sint32 priority = 0)
            : Operand(OperandType::Formula), mOperatorType(type), mOperatorPriority(priority) {mOperandP = nullptr;}
        public: ~Formula() {}

        // 가상 인터페이스
        public: void PrintString(String& collector) const override
        {
            auto PrintOperand = [](const OperandObject& operand, String& collector)->void
            {
                if(operand->type() == OperandType::Formula) collector += "(";
                operand->PrintString(collector);
                if(operand->type() == OperandType::Formula) collector += ")";
            };

            PrintOperand(mOperandL, collector);
            switch(mOperatorType)
            {
            case OperatorType::Addition:  collector += " + "; break;
            case OperatorType::Subtract:  collector += " - "; break;
            case OperatorType::Multiply:  collector += " * "; break;
            case OperatorType::Divide:    collector += " / "; break;
            case OperatorType::Remainder: collector += " % "; break;
            }
            PrintOperand(mOperandR, collector);
        }
        public: void UpdateChain(Solver* solver, SolverChain* chain) override
        {
            mOperandL->UpdateChain(solver, chain);
            mOperandR->UpdateChain(solver, chain);
        }
        public: virtual float reliable() const override
        {
            if(mOperatorType == OperatorType::Addition || mOperatorType == OperatorType::Subtract)
                return (mOperandL->reliable() + mOperandR->reliable()) / 2;
            return mOperandL->reliable() * mOperandR->reliable();
        }
        public: virtual SolverFloat result(SolverFloat zero) const override
        {
            if(0 < reliable())
            switch(mOperatorType)
            {
            case OperatorType::Addition:  return mOperandL->result(0) + mOperandR->result(0);
            case OperatorType::Subtract:  return mOperandL->result(0) - mOperandR->result(0);
            case OperatorType::Multiply:  return mOperandL->result(0) * mOperandR->result(1);
            case OperatorType::Divide:    return mOperandL->result(0) / mOperandR->result(1);
            case OperatorType::Remainder: return Math::Mod(mOperandL->result(0), mOperandR->result(1));
            }
            return zero;
        }
        public: buffer clone() const override
        {
            buffer NewBuffer = Buffer::AllocNoConstructorOnce<Formula>(BLIK_DBG 1);
            BLIK_CONSTRUCTOR(NewBuffer, 0, Formula, mOperatorType, mOperatorPriority);
            ((Formula*) NewBuffer)->mOperandL = mOperandL;
            ((Formula*) NewBuffer)->mOperandR = mOperandR;
            ((Formula*) NewBuffer)->mOperandP = mOperandP;
            return NewBuffer;
        }

        // 멤버
        public: const OperatorType mOperatorType; // 연산기호
        public: const sint32 mOperatorPriority; // 연산순위
        public: OperandObject mOperandL; // L항
        public: OperandObject mOperandR; // R항
        public: OperandObject* mOperandP; // 부모항
    };

    static uint32 gSolverUpdateId = 1;
    void SolverChainPair::ResetDest(Solver* solver, bool needupdate)
    {
        mDest = solver;
        if(needupdate)
        {
            gSolverUpdateId++; // UpdateId증가
            RenualAllObservers();
        }
    }

    bool SolverChainPair::RemoveDest()
    {
        mDest = nullptr;
        gSolverUpdateId++; // UpdateId증가
        RenualAllObservers();
        return (mObservers.Count() == 0);
    }

    void SolverChainPair::AddObserver(Solver* solver)
    {
        for(sint32 i = 0, iend = mObservers.Count(); i < iend; ++i)
        {
            if(!mObservers[i])
            {
                mObservers.At(i) = solver;
                return;
            }
        }
        mObservers.AtAdding() = solver;
    }

    bool SolverChainPair::SubObserver(Solver* solver)
    {
        for(sint32 i = 0, iend = mObservers.Count(); i < iend; ++i)
        {
            if(mObservers[i] == solver)
            {
                mObservers.At(i) = nullptr;
                break;
            }
        }
        // 배열길이 줄임
        while(0 < mObservers.Count() && !mObservers[-1])
            mObservers.SubtractionOne();
        return (!mDest && mObservers.Count() == 0);
    }

    void SolverChainPair::RenualAllObservers()
    {
        for(sint32 i = 0, iend = mObservers.Count(); i < iend; ++i)
            if(mObservers[i] && !mObservers[i]->is_result_matched(gSolverUpdateId))
            {
                gSolverUpdateId--; // Execute호출에 따른 UpdateId증가를 방지
                mObservers.At(i)->Execute();
            }
    }

    Solver::Solver()
    {
        mLinkedChain = nullptr;
        mReliable = 0;
        mResult = 0;
        mResultUpdateId = 0;
    }

    Solver::~Solver()
    {
        Unlink();
    }

    Solver& Solver::operator=(const Solver& rhs)
    {
        Unlink();
        mLinkedChain = rhs.mLinkedChain;
        mLinkedVariable = rhs.mLinkedVariable;
        mParsedFormula = rhs.mParsedFormula;
        mOperandTop = rhs.mOperandTop;
        mReliable = rhs.mReliable;
        mResult = rhs.mResult;
        mResultUpdateId = rhs.mResultUpdateId;

        // 강제적 권리이양
        ((Solver*) &rhs)->Unlink();
        ((Solver*) &rhs)->mOperandTop = OperandObject(true);
        if(mLinkedChain)
        {
            (*mLinkedChain)(mLinkedVariable).ResetDest(this, false);
            // 업데이드된 내용의 전달
            mOperandTop->UpdateChain(this, mLinkedChain);
        }
        return *this;
    }

    static Map<SolverChain> gSolverChains;
    void Solver::Link(chars chain, chars variable, bool needupdate)
    {
        Unlink();
        mLinkedChain = &gSolverChains(chain);
        mLinkedVariable = variable;
        (*mLinkedChain)(variable).ResetDest(this, needupdate);
        // 업데이드된 내용의 전달
        mOperandTop->UpdateChain(this, mLinkedChain);
    }

    void Solver::Unlink()
    {
        if(mLinkedChain)
        {
            if((*mLinkedChain)(mLinkedVariable).RemoveDest())
                mLinkedChain->Remove(mLinkedVariable);
            mLinkedChain = nullptr;
            mLinkedVariable = "";
        }
    }

    void Solver::Parse(chars formula)
    {
        auto AddOperator = [](OperandObject*& focus, OperatorType type, sint32 deep)->void
        {
            BLIK_ASSERT("잘못된 시나리오입니다", focus);
            const sint32 NewPriority = deep * 2 + (OperatorType::Multiply <= type);

            Formula NewFormula(type, NewPriority);
            if(focus->ConstPtr()->type() != OperandType::Formula) // 최초의 계산항이거나
            {
                NewFormula.mOperandL = *focus;
                *focus = NewFormula.clone();
            }
            else
            {
                Formula* CurFormula = (Formula*) focus->Ptr(); // 대상지 추적
                while(CurFormula->mOperandP && NewPriority <= ((const Formula*) CurFormula->mOperandP->ConstPtr())->mOperatorPriority)
                    CurFormula = (Formula*) (focus = CurFormula->mOperandP)->Ptr();

                if(CurFormula->mOperatorPriority < NewPriority) // 이전 연산기호보다 더 우선시되면
                {
                    NewFormula.mOperandL = CurFormula->mOperandR;
                    NewFormula.mOperandP = focus;
                    focus = &(CurFormula->mOperandR = NewFormula.clone());
                }
                else // 이전 연산기호와 같거나 차선시되면
                {
                    NewFormula.mOperandL = *focus;
                    NewFormula.mOperandP = CurFormula->mOperandP;
                    *focus = NewFormula.clone();
                }
            }
        };

        // 탑항을 초기화
        mOperandTop = OperandObject(true);
        OperandObject* OperandFocus = nullptr;
        bool OperatorTurn = false;
        for(sint32 deep = 0; *formula; ++formula)
        {
            // 공백과 괄호
            if(*formula == ' ');
            else if(*formula == '\t');
            else if(*formula == '\r');
            else if(*formula == '\n');
            else if(*formula == '(') deep++;
            else if(*formula == ')') deep--;
            else if(OperatorTurn) // 연산기호턴
            {
                // 계산항 밀어넣기
                if(*formula == '+') AddOperator(OperandFocus, OperatorType::Addition, deep);
                else if(*formula == '-') AddOperator(OperandFocus, OperatorType::Subtract, deep);
                else if(*formula == '*') AddOperator(OperandFocus, OperatorType::Multiply, deep);
                else if(*formula == '/') AddOperator(OperandFocus, OperatorType::Divide, deep);
                else if(*formula == '%') AddOperator(OperandFocus, OperatorType::Remainder, deep);
                else continue;
                OperatorTurn = false;
            }
            else // 피연산항턴
            {
                // 상수와 변수
                sint32 WordSize = 0;
                buffer NewOperand = nullptr;
                if(('0' <= *formula && *formula <= '9') || *formula == '+' || *formula == '-')
                    NewOperand = Literal(Parser::GetFloat<SolverFloat>(formula, -1, &WordSize)).clone();
                else
                {
                    chars End = formula;
                    while(*(++End))
                    {
                        switch(*End)
                        {
                        case ' ': case '\t': case '\r': case '\n': case '(': case ')':
                        case '+': case '-': case '*': case '/': case '%': break;
                        default: continue;
                        }
                        break;
                    }
                    NewOperand = Variable(String(formula, WordSize = End - formula)).clone();
                }
                formula += WordSize - 1;
                OperatorTurn = true;

                // 피연산항 밀어넣기
                if(OperandFocus)
                {
                    Formula* CurFormula = (Formula*) OperandFocus->Ptr();
                    BLIK_ASSERT("잘못된 시나리오입니다", CurFormula->type() == OperandType::Formula);
                    CurFormula->mOperandR = NewOperand;
                }
                else OperandFocus = &(mOperandTop = NewOperand);
            }
        }

        // 파싱결과를 기록
        mParsedFormula.Empty();
        mOperandTop->PrintString(mParsedFormula);
        // 체인을 업데이트
        mOperandTop->UpdateChain(this, mLinkedChain);
    }

    void Solver::Execute()
    {
        gSolverUpdateId++; // UpdateId증가
        const float OldReliable = mReliable;
        const SolverFloat OldResult = mResult;
        mReliable = mOperandTop->reliable();
        mResult = mOperandTop->result(0);

        // 현재 결과에 종속된 계산식들을 리뉴얼
        if(mLinkedChain)
        if(OldReliable != mReliable || OldResult != mResult)
        {
            mResultUpdateId = gSolverUpdateId;
            (*mLinkedChain)(mLinkedVariable).RenualAllObservers();
        }
    }
}
