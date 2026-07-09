#include "../include/PureBugReport.hpp"
#include "../include/PureState.hpp"

#include "clang/AST/Decl.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporter.h"

using namespace clang;
using namespace ento;

void PureBugReporter::reportImpureFunction(
    CheckerContext &C,
    const FunctionDecl *FD,
    unsigned SideEffects,
    const PureFunctionChecker *Checker)
{
    if (!FD)
        return;

    static std::unique_ptr<BugType> BT;
    if (!BT)
    {
        BT = std::make_unique<BugType>(
            Checker,
            "Function is not pure",
            "Pure Function Checker");
    }

    std::string Msg =
        "Function '" + FD->getNameAsString() +
        "' is annotated as pure but has a feasible execution with side effects:\n";

    bool First = true;

    auto AddEffect = [&](const char *Text)
    {
        if (!First)
            Msg += ", ";

        Msg += Text;
        First = false;
    };

    if (SideEffects & GlobalWrite)
        AddEffect("a global variable write");

    if (SideEffects & PointerWrite)
        AddEffect("a pointer write");

    if (SideEffects & ReferenceWrite)
        AddEffect("a reference write");

    if (SideEffects & UnknownCall)
        AddEffect("a call to a function with unknown purity");

    Msg += ".";

    emitReport(C, *BT, Msg);
}

void PureBugReporter::reportImpureCall(
    CheckerContext &C,
    const FunctionDecl *FD,
    const PureFunctionChecker *Checker)
{
    if (!FD)
        return;

    static std::unique_ptr<BugType> BT;
    if (!BT) {
        BT = std::make_unique<BugType>(
            Checker,
            "Pure function calls non-pure function",
            "Pure Function Checker");
    }

    std::string Msg = "Pure function calls non-pure function '" + FD->getNameAsString() + "'";

    emitReport(C, *BT, Msg);
}
void PureBugReporter::reportGlobalVariableUpdate(
    CheckerContext &C,
    const VarDecl *VD,
    const PureFunctionChecker *Checker)
{
    if (!VD)
        return;

    static std::unique_ptr<BugType> BT;
    if (!BT) {
        BT = std::make_unique<BugType>(
            Checker,
            "Pure function modifies global variable",
            "Pure Function Checker");
    }

    std::string Msg =
        "Pure function modifies global variable '" +
        VD->getNameAsString() +
        "'";

    emitReport(C, *BT, Msg);
}

void PureBugReporter::reportPointerWrite(
    CheckerContext &C,
    const PureFunctionChecker *Checker)
{
    static std::unique_ptr<BugType> BT;
    if (!BT) {
        BT = std::make_unique<BugType>(
            Checker,
            "Pointer write in pure function",
            "Pure Function Checker");
    }

    std::string Msg = "Pure function writes through a pointer";

    emitReport(C, *BT, Msg);
}

void PureBugReporter::reportReferenceWrite(
    CheckerContext &C,
    const ParmVarDecl *PVD,
    const PureFunctionChecker *Checker)
{
    if (!PVD)
        return;

    static std::unique_ptr<BugType> BT;
    if (!BT) {
        BT = std::make_unique<BugType>(
            Checker,
            "Reference write in pure function",
            "Pure Function Checker");
    }

    std::string Msg = "Pure function writes through reference parameter '" + PVD->getNameAsString() + "'";

    emitReport(C, *BT, Msg);
}

void PureBugReporter::emitReport(
    CheckerContext &C,
    BugType &BT,
    llvm::StringRef Message)
{
    ExplodedNode *N = C.generateNonFatalErrorNode();

    if (!N)
        return;

    auto Report =
        std::make_unique<PathSensitiveBugReport>(
            BT,
            Message,
            N);

    C.emitReport(std::move(Report));
}