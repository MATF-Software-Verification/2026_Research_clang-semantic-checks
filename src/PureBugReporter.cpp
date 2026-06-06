#include "../include/PureBugReport.hpp"

#include "clang/AST/Decl.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporter.h"

using namespace clang;
using namespace ento;

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