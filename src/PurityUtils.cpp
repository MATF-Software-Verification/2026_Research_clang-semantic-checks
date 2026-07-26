#include "../include/PurityUtils.hpp"
#include "../include/PureState.hpp"

#include "clang/AST/Attr.h"
#include "llvm/ADT/StringRef.h"

using namespace clang;
using namespace ento;

bool ento::isPureFunction(const FunctionDecl *FD) // returns true for both pure and const functions (const extends pure)
{
    if (!FD)
        return false;

    if (isConstFunction(FD))
        return true;

    for (const auto *Attr : FD->specific_attrs<AnnotateAttr>()) {
        if (Attr->getAnnotation() == "pure")
            return true;
    }

     return FD->hasAttr<PureAttr>();
}

bool ento::isConstFunction(const FunctionDecl *FD)
{
    if (!FD)
        return false;

    for (const auto *Attr : FD->specific_attrs<AnnotateAttr>()) {
        if (Attr->getAnnotation() == "const")
            return true;
    }

    return FD->hasAttr<ConstAttr>();
}