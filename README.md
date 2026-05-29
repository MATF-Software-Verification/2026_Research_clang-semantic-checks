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

- or their name starts with

```cpp
pure_
```

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
  ../tests/test.cpp
```