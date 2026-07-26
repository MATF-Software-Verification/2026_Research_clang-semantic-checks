#!/usr/bin/env bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 <llvm-build-dir> <const|pure|both>"
    exit 1
fi

LLVM_BUILD="$1"
MODE="$2"

case "$MODE" in
    const|pure|both)
        ;;
    *)
        echo "Invalid mode: $MODE"
        echo "Allowed modes: const, pure, both"
        exit 1
        ;;
esac

CHECKER=build/PureFunctionChecker.so
CLANG="$LLVM_BUILD/bin/clang"

for test in tests/*.cpp
do
    echo "========================"
    echo "Running $test with Mode=$MODE"
    echo "========================"

    "$CLANG" \
        --analyze \
        -Xclang -load \
        -Xclang "$CHECKER" \
        -Xclang -analyzer-checker=is-pure-fun \
        -Xclang -analyzer-config \
        -Xclang "is-pure-fun:Mode=$MODE" \
        "$test"

    echo
done