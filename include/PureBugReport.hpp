#pragma once

#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "../include/PureFunctionChecker.hpp"

#include <memory>

namespace clang
{
    namespace ento
    {

        class PureBugReporter
        {
        public:
            static void reportImpureCall(CheckerContext &C, const FunctionDecl *FD, const PureFunctionChecker *Checker);
            static void reportGlobalVariableUpdate(CheckerContext &C, const VarDecl *VD, const PureFunctionChecker *Checker);
            static void reportPointerWrite(CheckerContext &C, const PureFunctionChecker *Checker);
        };

    }
}