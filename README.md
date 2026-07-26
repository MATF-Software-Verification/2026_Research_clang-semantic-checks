# 2026_Research_clang-semantic-checks
# Pure Function Checker

Custom Clang Static Analyzer checker for detecting and validating pure functions.

Functions are recognized as pure if:

- they are annotated with

```cpp
[[clang::annotate("pure")]]
```

- they use the GNU pure attribute

```cpp
[[gnu::pure]]
```
Functions are recognized as const if:

- they are annotated with

```cpp
[[clang::annotate("const")]]
```

- they use the GNU const attribute

```cpp
[[gnu::const]]
```
## Current checks

For both `pure` and `const` functions the checker reports:

- writes to global variables
- writes through pointers
- writes through references
- calls to functions that violate purity requirements

Additionally, `const` functions may only call other `const` functions.

## Build

```bash
mkdir build
cd build

cmake .. \
  -DLLVM_DIR=/path/to/llvm-project/build/lib/cmake/llvm \
  -DClang_DIR=/path/to/llvm-project/build/lib/cmake/clang

make
```

## Run

```bash
/path/to/llvm-project/build/bin/clang \
  --analyze \
  -Xclang -load \
  -Xclang ./PureFunctionChecker.so \
  -Xclang -analyzer-checker=is-pure-fun \
  -Xclang -analyzer-config \
  -Xclang is-pure-fun:Mode=both \
  ../tests/test.cpp
```


Available modes:

- `pure` – checks only pure functions
- `const` – checks only const functions
- `both` – checks both pure and const functions

The default mode is `both`.

### Or run all tests

```bash
./tests/run_tests.sh path/to/llvm/build both
```

For example, to check only const functions:

```bash
./tests/run_tests.sh path/to/llvm/build const
```

To check only pure functions:

```bash
./tests/run_tests.sh path/to/llvm/build pure
```