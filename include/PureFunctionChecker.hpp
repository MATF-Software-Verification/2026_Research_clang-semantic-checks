#pragma once

#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"

namespace clang
{
    namespace ento
    {
        enum class CheckerMode
        {
            Pure,
            Const,
            Both
        };

        enum FunctionKind : unsigned
        {
            NoFunctionKind = 0,
            PureFunctionKind,
            ConstFunctionKind
        };

        enum SideEffectKind : unsigned
        {
            NoSideEffect = 0,
            GlobalWrite = 1 << 0,
            PointerWrite = 1 << 1,
            ReferenceWrite = 1 << 2,
            UnknownCall = 1 << 3,
            InsufficientlyPureCall = 1 << 4,
            GlobalRead = 1 << 5
        };

        class PureFunctionChecker
            : public Checker<
                check::BeginFunction,
                check::EndFunction,
                check::PreCall,
                check::Bind,
                check::Location>
        {

        public:
            void setMode(CheckerMode NewMode)
            {
                Mode = NewMode;
            }
            void checkBeginFunction(CheckerContext &C) const;
            void checkEndFunction(const ReturnStmt *RS, CheckerContext &C) const;
            void checkPreCall(const CallEvent &Call, CheckerContext &C) const;
            void checkBind(SVal Loc, SVal Val, const Stmt *S, bool AtDeclInit, CheckerContext &C) const;
            void checkLocation(SVal Loc, bool IsLoad, const Stmt *S, CheckerContext &C) const;

        private:
            CheckerMode Mode = CheckerMode::Both;
            bool shouldCheckFunction(const FunctionDecl *FD) const;
            ProgramStateRef addSideEffect(ProgramStateRef State, SideEffectKind Kind) const;
            bool isPointerWrite(const Stmt *Stmt) const;
            bool isReferenceWrite(const Stmt *Stmt) const;
            bool isGlobalWrite(SVal Loc) const;
            bool isGlobalRead(SVal Loc) const;
        };

    }
}

