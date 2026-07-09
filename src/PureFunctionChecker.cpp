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

    unsigned depth = State->get<PureDepth>();

    State = State->set<PureDepth>(depth + 1);

    State = State->set<SideEffectsAtDepth>(depth + 1, SideEffectKind::NoSideEffect);


    C.addTransition(State);
}

void PureFunctionChecker::checkEndFunction(
    const ReturnStmt *,
    CheckerContext &C) const
{
    ProgramStateRef State = C.getState();

    unsigned Depth = State->get<PureDepth>();

    if (Depth == 0)
        return;

    const auto *FD = dyn_cast<FunctionDecl>(C.getLocationContext()->getDecl());

    if (!FD)
        return;

    if (!isPureFunction(FD))
        return;

    const unsigned *Effects = State->get<SideEffectsAtDepth>(Depth);

    if (Effects && *Effects != SideEffectKind::NoSideEffect) {

        PureBugReporter::reportImpureFunction(C, FD, *Effects, this);

        if (Depth > 1){
            const unsigned *ParentEffects = State->get<SideEffectsAtDepth>(Depth - 1);

            unsigned ParentMask = ParentEffects ? *ParentEffects : NoSideEffect;

            ParentMask |= *Effects;

            State = State->set<SideEffectsAtDepth>(Depth - 1, ParentMask);
        }
    }

    State = State->remove<SideEffectsAtDepth>(Depth);

    State = State->set<PureDepth>(Depth - 1);

    C.addTransition(State);
}

void PureFunctionChecker::checkPreCall(
    const CallEvent &Call,
    CheckerContext &C) const
{
    ProgramStateRef State = C.getState();

    if (!(State->get<PureDepth>() > 0))
        return;

    const auto *FD = dyn_cast_or_null<FunctionDecl>(Call.getDecl());

    if (!FD)
        return;

    if (FD->hasBody())
        return;

    if (isPureFunction(FD))
        return;

    State = addSideEffect(State, SideEffectKind::UnknownCall);

    C.addTransition(State);
}

void PureFunctionChecker::checkBind(
    SVal Loc, SVal Val, const Stmt *S, bool AtDeclInit, CheckerContext &C) const
{
    ProgramStateRef State = C.getState();

    if (!(State->get<PureDepth>() > 0))
        return;

    checkPointerWrite(S, C);
    checkReferenceWrite(S, C);

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

void PureFunctionChecker::checkPointerWrite(
    const Stmt *Stmt,
    CheckerContext &C) const
{
    const auto *BO = dyn_cast<BinaryOperator>(Stmt);

    if (!BO)
        return;

    const Expr *LHS = BO->getLHS()->IgnoreParenImpCasts();

    const auto *UO = dyn_cast<UnaryOperator>(LHS);

    if (!UO)
        return;

    if (UO->getOpcode() != UO_Deref)
        return;

    ProgramStateRef State = C.getState();

    State = addSideEffect(State, SideEffectKind::PointerWrite);

    C.addTransition(State);
}

void PureFunctionChecker::checkReferenceWrite(
    const Stmt *Stmt,
    CheckerContext &C) const
{
    const auto *BO = dyn_cast<BinaryOperator>(Stmt);

    if (!BO)
        return;

    const Expr *LHS = BO->getLHS()->IgnoreParenImpCasts();

    const auto *DRE = dyn_cast<DeclRefExpr>(LHS);

    if (!DRE)
        return;

    const auto *PVD = dyn_cast<ParmVarDecl>(DRE->getDecl());

    if (!PVD)
        return;

    if (!PVD->getType()->isReferenceType())
        return;

    ProgramStateRef State = C.getState();

    State = addSideEffect(State, SideEffectKind::ReferenceWrite);

    C.addTransition(State);
}

ProgramStateRef PureFunctionChecker::addSideEffect(
    ProgramStateRef State,
    SideEffectKind Kind) const
{
    unsigned Depth = State->get<PureDepth>();

    if (Depth == 0)
        return State;

    const unsigned *Effects = State->get<SideEffectsAtDepth>(Depth);

    unsigned Mask = Effects ? *Effects : NoSideEffect;

    Mask |= static_cast<unsigned>(Kind);

    return State->set<SideEffectsAtDepth>(Depth, Mask);
}