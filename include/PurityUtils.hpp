#pragma once

#include "clang/AST/Decl.h"

namespace clang
{
    namespace ento
    {

        bool isPureFunction(const FunctionDecl *FD);

    }
}