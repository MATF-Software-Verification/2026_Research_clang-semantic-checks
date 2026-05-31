#include "../include/PurityUtils.hpp"
#include "../include/PureState.hpp"

#include "clang/AST/Attr.h"
#include "llvm/ADT/StringRef.h"

using namespace clang;
using namespace ento;

bool ento::isPureFunction(const FunctionDecl *FD)
{
    if (!FD)
        return false;

    if (FD->hasAttr<PureAttr>())
        return true;

    for (const auto *Attr : FD->specific_attrs<AnnotateAttr>()) {
        if (Attr->getAnnotation() == "pure")
            return true;
    }

    llvm::StringRef Name = FD->getName();

    if (Name.starts_with("pure_"))
        return true;

    return false;
}


bool ento::isInsidePureFunction(ProgramStateRef State)
{
    return State->get<PureDepth>() > 0;
}

ProgramStateRef ento::enterPureFunction(ProgramStateRef State)
{
    unsigned depth = State->get<PureDepth>();

    return State->set<PureDepth>(depth + 1);
}

ProgramStateRef ento::leavePureFunction(ProgramStateRef State)
{
    unsigned depth = State->get<PureDepth>();

    if (Depth == 0)
        return State;

    return State->set<PureDepth>(depth - 1);
}