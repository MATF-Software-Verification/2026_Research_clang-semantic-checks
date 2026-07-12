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
            static void reportImpureFunction(CheckerContext &C, const FunctionDecl *FD, unsigned SideEffects, const PureFunctionChecker *Checker);
        };

    }
}