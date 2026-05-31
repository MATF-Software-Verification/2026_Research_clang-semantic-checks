#pragma once

#include "clang/AST/Decl.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"

namespace clang
{
    namespace ento
    {

        bool isPureFunction(const FunctionDecl *FD);
        bool isInsidePureFunction(ProgramStateRef State);
        ProgramStateRef enterPureFunction(ProgramStateRef State);
        ProgramStateRef leavePureFunction(ProgramStateRef State);
    }
}