#pragma once

#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramStateTrait.h"

REGISTER_TRAIT_WITH_PROGRAMSTATE(PureDepth, unsigned)

REGISTER_MAP_WITH_PROGRAMSTATE(
    SideEffectsAtDepth,
    unsigned,
    bool)