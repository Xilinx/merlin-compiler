# LibTooling static link config for Merlin.

cmake_minimum_required(VERSION 3.4)

execute_process(
    COMMAND "grep" "^processor" "/proc/cpuinfo"
    COMMAND "wc" "-l"
    COMMAND "tr" "-d" "'\n'"
    OUTPUT_VARIABLE ncpu
)

set(GIT_ORIGIN "file://$ENV{HOME}/usr/git" CACHE STRING "")

message("Building with ${ncpu} cpus")
set(LLVM_PARALLEL_COMPILE_JOBS ${ncpu} CACHE STRING "")
set(LLVM_PARALLEL_LINK_JOBS ${ncpu} CACHE STRING "")

# link statically
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")

set(CMAKE_C_COMPILER "gcc" CACHE PATH "")
set(CMAKE_CXX_COMPILER "g++" CACHE PATH "")

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
set(CMAKE_C_FLAGS "-O3" CACHE STRING "")
set(CMAKE_CXX_FLAGS "-O3" CACHE STRING "")
set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/usr" CACHE PATH "")
set(LLVM_TARGETS_TO_BUILD "X86" CACHE STRING "")
set(LLVM_BUILD_RUNTIME OFF CACHE BOOL "")
set(LLVM_ENABLE_ASSERTIONS OFF CACHE BOOL "")

# Merlin diags don't require color or zlib. This sheds two extra runtime deps.
set(LLVM_ENABLE_TERMINFO OFF CACHE BOOL "")
set(LLVM_ENABLE_ZLIB OFF CACHE BOOL "")

set(LLVM_TOOL_CLANG_BUILD ON CACHE BOOL "")

set(CLANG_DEFAULT_LINKER "lld" CACHE BOOL "")
set(CLANG_DEFAULT_CXX_STDLIB "libc++" CACHE STRING "")
