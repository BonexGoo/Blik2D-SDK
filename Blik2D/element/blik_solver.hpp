#pragma once
#include <blik.hpp>

namespace BLIK
{
    class Solver;
    #define SolverFloat double
    enum class OperandType {Unknown, Literal, Variable, Formula};
    enum class OperatorType {Unknown, Addition, Subtract, Multiply, Divide, Remainder};

    // 업데이트체인
    class SolverChainPair
    {
        public: SolverChainPair() {mDest = nullptr;}
        public: ~SolverChainPair() {}

        // 인터페이스
        public: void ResetDest(Solver* solver, bool needupdate);
        public: bool RemoveDest();
        public: void AddObserver(Solver* solver);
        public: bool SubObserver(Solver* solver);
        public: void RenualAllObservers();
        public: const Solver* dest() const {return mDest;}

        // 멤버
        private: Solver* mDest;
        private: Array<Solver*, datatype_pod_canmemcpy_zeroset> mObservers;
    };
    typedef Map<SolverChainPair> SolverChain;

    // 피연산항
    class Operand
    {
        BLIK_DECLARE_NONCOPYABLE_INITIALIZED_CLASS(Operand, mOperandType(rhs.mOperandType))
        public: Operand(OperandType type = OperandType::Unknown) : mOperandType(type) {}
        public: ~Operand() {}

        // 인터페이스
        public: inline OperandType type() const {return mOperandType;}
        // 가상 인터페이스
        public: virtual void PrintString(String& collector) const = 0; // 데이터출력
        public: virtual void UpdateChain(Solver* solver, SolverChain* chain) = 0; // 체인업데이트
        public: virtual float reliable() const = 0; // 신뢰도
        public: virtual SolverFloat result(SolverFloat zero) const = 0; // 결과값
        public: virtual buffer clone() const = 0; // 자기복제

        // 멤버
        protected: const OperandType mOperandType;
    };
    class OperandBlank : public Operand
    {
        public: void PrintString(String& collector) const override {}
        public: void UpdateChain(Solver* solver, SolverChain* chain) override {}
        public: float reliable() const override {return 0;}
        public: SolverFloat result(SolverFloat zero) const override {return zero;}
        public: buffer clone() const override {return nullptr;}
    };
    typedef Object<OperandBlank, datatype_class_nomemcpy, true> OperandObject;
    typedef Array<OperandObject> OperandObjects;

    // 연립방정식
    class Solver
    {
        public: Solver();
        public: ~Solver();
        public: Solver(const Solver& rhs) {operator=(rhs);}
        public: Solver& operator=(const Solver& rhs);

        // 인터페이스
        public: void Link(chars chain, chars variable, bool needupdate);
        public: void Unlink();
        public: void Parse(chars formula);
        public: void Execute();
        public: inline const String& linked_variable() const {return mLinkedVariable;}
        public: inline const String& parsed_formula() const {return mParsedFormula;}
        public: inline float reliable() const {return mReliable;}
        public: inline SolverFloat result() const {return mResult;}
        public: inline bool result_updated() const {return mResultUpdated;}
        public: inline void ClearUpdateMark() {mResultUpdated = false;}

        // 멤버
        private: SolverChain* mLinkedChain;
        private: String mLinkedVariable;
        private: String mParsedFormula;
        private: OperandObject mOperandTop;
        private: float mReliable;
        private: SolverFloat mResult;
        private: bool mResultUpdated;
    };
    typedef Array<Solver> Solvers;
}
