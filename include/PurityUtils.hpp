#pragma once

#include "clang/AST/Decl.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"

namespace clang
{
    namespace ento
    {
        bool isPureFunction(const FunctionDecl *FD);
        bool isConstFunction(const FunctionDecl *FD);
    }
}