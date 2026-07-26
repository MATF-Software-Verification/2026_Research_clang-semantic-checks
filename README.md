# 2026_Research_clang-semantic-checks
# Pure Function Checker

Custom Clang Static Analyzer checker for detecting and validating `pure` and `const` functions.

## Authors

- Anja Milutinović 1011/2025
- Đurđa Milošević 1008/2025

## Requirements

The project depends on LLVM and Clang.

Clone the LLVM project repository:

```bash
git clone https://github.com/llvm/llvm-project.git
```

Build LLVM and Clang according to the official LLVM build instructions.

The project requires:

- LLVM
- Clang Static Analyzer
- CMake
- C++17 compatible compiler

## Build

Create a build directory and configure the project:

```bash
mkdir build
cd build

cmake .. \
  -DLLVM_DIR=/path/to/llvm-project/build/lib/cmake/llvm \
  -DClang_DIR=/path/to/llvm-project/build/lib/cmake/clang

make
```

This produces the shared library:

```text
PureFunctionChecker.so
```

## Usage

Run the checker using Clang Static Analyzer:

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

- `both` – checks both pure and const functions (default)

## Input examples

Example input files are provided in the `tests/` directory.

The checker supports functions annotated with:

```cpp
[[gnu::pure]]
[[gnu::const]]
[[clang::annotate("pure")]]
[[clang::annotate("const")]]
```

## Implemented checks

For both `pure` and `const` functions the checker reports:

- writes to global variables
- writes through pointers
- writes through references
- calls to functions that violate purity requirements

Additionally, `const` functions may only call other `const` functions.

## Testing

All test cases are located in the `tests/` directory.

To run all tests:

```bash
./tests/run_tests.sh /path/to/llvm/build both
```

To test only `const` functions:

```bash
./tests/run_tests.sh /path/to/llvm/build const
```

To test only `pure` functions:

```bash
./tests/run_tests.sh /path/to/llvm/build pure
```

## Tools

The project was developed using:

- LLVM
- Clang Static Analyzer
- CMake
- Git