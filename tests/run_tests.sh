#!/usr/bin/env bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <llvm-build-dir>"
    exit 1
fi

LLVM_BUILD="$1"

CHECKER=build/PureFunctionChecker.so
CLANG="$LLVM_BUILD/bin/clang"

for test in tests/*.cpp
do
    echo "========================"
    echo "Running $test"
    echo "========================"

    "$CLANG" \
        --analyze \
        -Xclang -load \
        -Xclang "$CHECKER" \
        -Xclang -analyzer-checker=is-pure-fun \
        "$test"

    echo
done