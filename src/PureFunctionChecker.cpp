#include "../include/PureFunctionChecker.hpp"
#include "../include/PurityUtils.hpp"
#include "../include/PureState.hpp"
#include "../include/PureBugReport.hpp"

#include "clang/AST/Decl.h"
#include "clang/StaticAnalyzer/Core/CheckerManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramStateTrait.h"
#include "clang/StaticAnalyzer/Frontend/CheckerRegistry.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"

using namespace clang;
using namespace ento;

extern "C" const char clang_analyzerAPIVersionString[] =
    CLANG_ANALYZER_API_VERSION_STRING;


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

    State = enterPureFunction(State);

    C.addTransition(State);
}

void PureFunctionChecker::checkEndFunction(
    const ReturnStmt *,
    CheckerContext &C) const
{
    ProgramStateRef State = C.getState();

    if (!State->get<PureDepth>())
        return;

    State = leavePureFunction(State);

    C.addTransition(State);
}

void PureFunctionChecker::checkPreCall(
    const CallEvent &Call,
    CheckerContext &C) const
{
    ProgramStateRef State = C.getState();

    if (!isInsidePureFunction(State))
        return;

    const auto *FD = dyn_cast_or_null<FunctionDecl>(Call.getDecl());

    if (!FD)
        return;

    if (!FD->hasBody())
        return;

    if (isPureFunction(FD))
        return;

    PureBugReporter::reportImpureCall(C, FD, this);
    /*
    llvm::errs()
        << "PURE FUNCTION CALLS NON-PURE FUNCTION: "
        << FD->getNameAsString()
        << "\n";
    */
        
}
void PureFunctionChecker::checkBind(
    SVal Loc, SVal Val, const Stmt *S, bool AtDeclInit, CheckerContext &C) const
{
    ProgramStateRef State = C.getState();

    if (!isInsidePureFunction(State))
        return;

    const MemRegion *MR = Loc.getAsRegion();

    if (!MR)
        return;

    const auto *VR = MR->getBaseRegion()->getAs<VarRegion>();

    if (!VR)
        return;

    const VarDecl *VD = VR->getDecl();

    if (!VD)
        return;

    if (!VD->hasGlobalStorage())
        return;

    PureBugReporter::reportGlobalVariableUpdate(C, VD, this);
        
}

extern "C" void clang_registerCheckers(CheckerRegistry &registry)
{
    registry.addChecker<PureFunctionChecker>(
        "is-pure-fun",
        "Checks functions annotated as pure for possible side effects",
        "");
}