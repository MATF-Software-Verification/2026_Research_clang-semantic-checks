#include "../include/PureBugReport.hpp"

#include "clang/AST/Decl.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporter.h"

using namespace clang;
using namespace ento;

void PureBugReporter::reportImpureCall(
    CheckerContext &C,
    const FunctionDecl *FD,
    const CheckerFrontend *Checker)
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

    ExplodedNode *N = C.generateNonFatalErrorNode();
    if (!N)
        return;

    std::string Msg = "Pure function calls non-pure function '" + FD->getNameAsString() + "'";

    auto Report =std::make_unique<PathSensitiveBugReport>(*BT, Msg, N);

    C.emitReport(std::move(Report));
}