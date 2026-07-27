#include "../include/PureFunctionChecker.hpp"
#include "../include/PurityUtils.hpp"
#include "../include/PureState.hpp"
#include "../include/PureBugReport.hpp"

#include "clang/AST/Decl.h"
#include "clang/StaticAnalyzer/Core/AnalyzerOptions.h"
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

    if (!shouldCheckFunction(FD))
        return;
    
    FunctionKind Kind = NoFunctionKind;

    if (isConstFunction(FD))
        Kind = ConstFunctionKind;
    else if (isPureFunction(FD))
        Kind = PureFunctionKind;

    if (Kind == NoFunctionKind)
        return;

    unsigned depth = State->get<PureDepth>();

    State = State->set<PureDepth>(depth + 1);

    State = State->set<SideEffectsAtDepth>(depth + 1, SideEffectKind::NoSideEffect);

    State = State->set<FunctionKindsAtDepth>(depth + 1, static_cast<unsigned>(Kind));

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

    if (!shouldCheckFunction(FD))
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
    State = State->remove<FunctionKindsAtDepth>(Depth);

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

    const unsigned *Kind = State->get<FunctionKindsAtDepth>(State->get<PureDepth>());

    if (!Kind)
        return;

    // const can call only const function (calling pure is not safe by default), while pure can call both pure and const functions

    if (*Kind == ConstFunctionKind) {
        if (isConstFunction(FD))
            return;

        if (isPureFunction(FD)) {
            State = addSideEffect(State, SideEffectKind::InsufficientlyPureCall);
        }
        else {
            State = addSideEffect(State, SideEffectKind::UnknownCall);
        }
    }
    else {
        if (isPureFunction(FD))
            return;

        State = addSideEffect(State, SideEffectKind::UnknownCall);
    }

    C.addTransition(State);
}

void PureFunctionChecker::checkBind(
    SVal Loc, SVal Val, const Stmt *S, bool AtDeclInit, CheckerContext &C) const
{
    ProgramStateRef State = C.getState();

    if (!(State->get<PureDepth>() > 0))
        return;

    unsigned DetectedEffects = SideEffectKind::NoSideEffect;

    if (isGlobalWrite(Loc))
        DetectedEffects |= SideEffectKind::GlobalWrite;

    if (isPointerWrite(S))
        DetectedEffects |= SideEffectKind::PointerWrite;

    if (isReferenceWrite(S))
        DetectedEffects |= SideEffectKind::ReferenceWrite;

    if (DetectedEffects == SideEffectKind::NoSideEffect)
        return;

    const unsigned Depth = State->get<PureDepth>();

    const unsigned *CurrentEffects =
        State->get<SideEffectsAtDepth>(Depth);

    unsigned UpdatedMask =
        CurrentEffects
            ? *CurrentEffects
            : SideEffectKind::NoSideEffect;

    UpdatedMask |= DetectedEffects;

    State = State->set<SideEffectsAtDepth>(Depth, UpdatedMask);

    C.addTransition(State);
        
}

static void registerPureFunctionChecker(
    CheckerManager &Mgr)
{
    auto *Checker = Mgr.registerChecker<PureFunctionChecker>();

    const AnalyzerOptions &Options = Mgr.getAnalyzerOptions();

    const auto Mode = Options.getCheckerStringOption(Checker, "Mode");

    if (Mode == "pure")
        Checker->setMode(CheckerMode::Pure);
    else if (Mode == "const")
        Checker->setMode(CheckerMode::Const);
    else
        Checker->setMode(CheckerMode::Both);
}
static bool shouldRegisterPureFunctionChecker(
    const CheckerManager &)
{
    return true;
}

extern "C" void clang_registerCheckers(CheckerRegistry &registry)
{
    registry.addChecker(
        &registerPureFunctionChecker,
        &shouldRegisterPureFunctionChecker,
        "is-pure-fun",
        "Checks pure and const function requirements",
        "NoDocsUri",
        false);

    registry.addCheckerOption(
        "string",
        "is-pure-fun",
        "Mode",
        "both",
        "Select functions to check: pure, const, or both",
        "released");
}

bool PureFunctionChecker::isGlobalWrite(SVal Loc) const
{
    const MemRegion *Region = Loc.getAsRegion();

    if (!Region)
        return false;

    const auto *VarRegion =
        Region->getBaseRegion()->getAs<clang::ento::VarRegion>();

    if (!VarRegion)
        return false;

    const VarDecl *VD = VarRegion->getDecl();

    return VD && VD->hasGlobalStorage();
}

bool PureFunctionChecker::isPointerWrite(const Stmt *Stmt) const
{
    const auto *BO = dyn_cast<BinaryOperator>(Stmt);

    if (!BO || !BO->isAssignmentOp())
        return false;

    const Expr *LHS = BO->getLHS()->IgnoreParenImpCasts();

    const auto *UO = dyn_cast<UnaryOperator>(LHS);

    return UO && UO->getOpcode() == UO_Deref;
}

bool PureFunctionChecker::isReferenceWrite(const Stmt *Stmt) const
{
    const auto *BO = dyn_cast<BinaryOperator>(Stmt);

    if (!BO || !BO->isAssignmentOp())
        return false;

    const Expr *LHS = BO->getLHS()->IgnoreParenImpCasts();

    const auto *DRE = dyn_cast<DeclRefExpr>(LHS);

    if (!DRE)
        return false;

    const auto *PVD = dyn_cast<ParmVarDecl>(DRE->getDecl());

    return PVD && PVD->getType()->isReferenceType();
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
bool PureFunctionChecker::shouldCheckFunction(const FunctionDecl *FD) const
{
    if (!FD)
        return false;

    switch (Mode)
    {
    case CheckerMode::Pure:
        return isPureFunction(FD) && !isConstFunction(FD);

    case CheckerMode::Const:
        return isConstFunction(FD);

    case CheckerMode::Both:
        return isPureFunction(FD);
    }

    return false;
}

void PureFunctionChecker::checkLocation(
    SVal Loc,
    bool IsLoad,
    const Stmt *S,
    CheckerContext &C) const
{
    if (!IsLoad)
        return;

    ProgramStateRef State = C.getState();

    if (State->get<PureDepth>() == 0)
        return;

    const unsigned *Kind = State->get<FunctionKindsAtDepth>(State->get<PureDepth>());

    if (!Kind || *Kind != ConstFunctionKind)
        return;

    if (!isGlobalRead(Loc))
        return;

    State = addSideEffect(State, SideEffectKind::GlobalRead);

    C.addTransition(State);
}

bool PureFunctionChecker::isGlobalRead(SVal Loc) const
{
    const MemRegion *Region = Loc.getAsRegion();

    if (!Region)
        return false;

    const auto *VarRegion =
        Region->getBaseRegion()->getAs<clang::ento::VarRegion>();

    if (!VarRegion)
        return false;

    return VarRegion->getDecl()->hasGlobalStorage();
}