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

    ExplodedNode *N = C.generateNonFatalErrorNode();

    if (!N)
        return;

    auto Report = std::make_unique<PathSensitiveBugReport>(*BT, Msg, N);

    C.emitReport(std::move(Report));
}