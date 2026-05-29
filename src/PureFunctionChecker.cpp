#include "../include/PureFunctionChecker.hpp"
#include "../include/PurityUtils.hpp"

#include "clang/AST/Decl.h"
#include "clang/StaticAnalyzer/Core/CheckerManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramStateTrait.h"
#include "clang/StaticAnalyzer/Frontend/CheckerRegistry.h"

using namespace clang;
using namespace ento;

extern "C" const char clang_analyzerAPIVersionString[] =
    CLANG_ANALYZER_API_VERSION_STRING;

REGISTER_TRAIT_WITH_PROGRAMSTATE(InsidePureFunction, bool)

void PureFunctionChecker::checkBeginFunction(
    CheckerContext &C) const
{
    ProgramStateRef State = C.getState();

    const LocationContext *LCtx = C.getLocationContext();

    const auto *FD = dyn_cast<FunctionDecl>(LCtx->getDecl());

    if (!FD)
        return;

    if (!isPureFunction(FD))
        return;

    llvm::errs() << "Entered PURE function: "
                 << FD->getNameAsString()
                 << "\n";

    State = State->set<InsidePureFunction>(true);

    C.addTransition(State);
}

extern "C" void clang_registerCheckers(CheckerRegistry &registry)
{
    registry.addChecker<PureFunctionChecker>(
        "is-pure-fun",
        "Checks functions annotated as pure for possible side effects",
        "");
}