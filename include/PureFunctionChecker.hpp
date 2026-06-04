#pragma once

#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"

namespace clang
{
    namespace ento
    {

        class PureFunctionChecker
            : public Checker<
                check::BeginFunction,
                check::EndFunction,
                check::PreCall,
                check::Bind>
        {

        public:
            void checkBeginFunction(CheckerContext &C) const;
            void checkEndFunction(const ReturnStmt *RS, CheckerContext &C) const;
            void checkPreCall(const CallEvent &Call, CheckerContext &C) const;
            void checkBind(SVal Loc, SVal Val, const Stmt *S, bool AtDeclInit, CheckerContext &C) const;

        private:
            void checkPointerWrite(const Stmt *Stmt, CheckerContext &C) const;
            void checkReferenceWrite(const Stmt *Stmt, CheckerContext &C) const;
        };

    }
}