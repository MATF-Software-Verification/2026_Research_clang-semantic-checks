#pragma once

#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/Checker.h"

#include <memory>

namespace clang
{
    namespace ento
    {

        class PureBugReporter
        {
        public:
            static void reportImpureCall(CheckerContext &C, const FunctionDecl *FD, const CheckerFrontend *Checker);
        };

    }
}