#pragma once
#include <service/blik_zay.hpp>
#include <element/blik_solver.hpp>

class linecalcData : public ZayObject
{
public:
    linecalcData();
    ~linecalcData();

public:
    void DeleteSolver(sint32 i);

public:
    String CurVariable;
    String CurFormula;
    Solvers CollectedSolvers;
};
