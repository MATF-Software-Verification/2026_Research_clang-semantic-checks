cmake_minimum_required(VERSION 3.13)

project(PureFunctionChecker)

set(CMAKE_CXX_STANDARD 17)

set(LLVM_DIR ~/Desktop/llvm-project/build/lib/cmake/llvm)
set(Clang_DIR ~/Desktop/llvm-project/build/lib/cmake/clang)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include(AddLLVM)

include_directories(
    ${LLVM_INCLUDE_DIRS}
    ${CLANG_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_definitions(${LLVM_DEFINITIONS})

add_llvm_library(PureFunctionChecker MODULE
    src/PureFunctionChecker.cpp
    src/PurityUtils.cpp

    PLUGIN_TOOL
    clang
)
target_include_directories(PureFunctionChecker
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)