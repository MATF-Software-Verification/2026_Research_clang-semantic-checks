#pragma once

#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"

namespace clang
{
    namespace ento
    {

        class PureFunctionChecker
            : public Checker<check::BeginFunction>
        {

        public:
            void checkBeginFunction(CheckerContext &C) const;
        };

    }
}