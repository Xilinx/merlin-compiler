cmake_minimum_required(VERSION 3.10)

# a macro to load dependencies from environment
macro(LoadEnv var desc)
if(DEFINED ENV{${var}})
  set(${var} "$ENV{${var}}")
  message(STATUS "Found ${desc} environment at ${var}, which is $ENV{${var}}")
elseif(DEFINED ${var})
  # support passing this variable from commandline with '-D'
  message(STATUS "Found ${desc} environment at ${var}")
else()
  message(FATAL_ERROR "Cannot find ${desc} environment, please set ${var}")
endif()
endmacro(LoadEnv)

LoadEnv("LLVM_ROOT_DIR"       "LLVM")
LoadEnv("GCC_ROOT_DIR"        "GCC")
LoadEnv("BOOST_ROOT"          "BOOST")
LoadEnv("CLANG_ROOT_DIR"      "Clang")
LoadEnv("ROSE_ROOT_DIR"       "ROSE")
LoadEnv("RAPIDJSON_ROOT_DIR"  "RapidJSON")

# first locate clang compiler
set(CMAKE_C_COMPILER "${LLVM_ROOT_DIR}/bin/clang")
set(CMAKE_CXX_COMPILER "${LLVM_ROOT_DIR}/bin/clang++")

project(Merlin)

# For FindLLVM and FindBoost
find_package(Boost 1.67 REQUIRED
              COMPONENTS regex filesystem thread system)
#find_package(JNI REQUIRED)
find_package(Threads)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
find_package(LLVM REQUIRED)

## Google Test
#configure_file(${CMAKE_MODULE_PATH}/FindGoogleTest.cmake googletest-download/CMakeLists.txt)
#execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#  RESULT_VARIABLE result
#  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
#if(result)
#  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
#endif()
#execute_process(COMMAND ${CMAKE_COMMAND} --build .
#  RESULT_VARIABLE result
#  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
#if(result)
#  message(FATAL_ERROR "Build step for googletest failed: ${result}")
#endif()
#
## Prevent overriding the parent project's compiler/linker
## settings on Windows
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#
## Add googletest directly to our build. This defines
## the gtest and gtest_main targets.
#add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
#                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
#                 EXCLUDE_FROM_ALL)
#
## The gtest/gtest_main targets carry header search path
## dependencies automatically when using CMake 2.8.11 or
## later. Otherwise we have to add them here ourselves.
#if (CMAKE_VERSION VERSION_LESS 2.8.11)
#  include_directories("${gtest_SOURCE_DIR}/include")
#endif()

# find docker executable
#find_program(Docker_EXECUTABLE docker)
#if(NOT Docker_EXECUTABLE)
#  message(FATAL_ERROR "Cannot find the docker executable")
#endif()

set(MARS_GEN_TOP "${PROJECT_SOURCE_DIR}/mars-gen")
set(ESLOPT_TOP "${PROJECT_SOURCE_DIR}/legacy")
set(MARS_TOP "${PROJECT_SOURCE_DIR}/source-opt")
set(OPTIMIZER_HOME "${PROJECT_SOURCE_DIR}/optimizers")
set(TOOLS_TOP "${ESLOPT_TOP}/tools")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# default the build type to release
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

# get version str from git
execute_process(
    COMMAND git describe --tags --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND git rev-list --count HEAD 
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_REV_COUNT
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(
    COMMAND date 
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE BUILD_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE)

set(BUILD_VERSION "${GIT_VERSION}.${GIT_REV_COUNT}")
message(STATUS "build version: ${BUILD_VERSION}")
message(STATUS "build version date: ${BUILD_DATE}")

set(CMAKE_CXX_FLAGS "-Wall -Wno-c++17-extensions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILD_VERSION=\"\\\"${BUILD_VERSION}\"\\\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBUILD_DATE=\"\\\"${BUILD_DATE}\"\\\"")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --gcc-toolchain=${GCC_ROOT_DIR}")


set(CMAKE_CXX_FLAGS_DEBUG "-DPROJDEBUG=1 -std=c++14 -O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-DPROJDEBUG=1 -std=c++14 -O3")

option(GLIBXX_DEBUG  "Build with STL checking" OFF) 
if(GLIBXX_DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG")
    set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -D_GLIBCXX_DEBUG")
endif()
  
option(SANITIZE "Build with address and UB sanitizers" OFF)
if(SANITIZE)
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined")
endif()

option(COVERAGE "Build with coverage report" OFF)
if(COVERAGE)
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

option(PROFILING "Build with profiling report" OFF)
if(PROFILING)
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()


# Let's dump all the useful artifacts in build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory("legacy")
#add_subdirectory("flexlm")
add_subdirectory("${MARS_GEN_TOP}/tools")
add_subdirectory("${MARS_TOP}/tools")
add_subdirectory("${OPTIMIZER_HOME}")

add_custom_target(version
    ALL
    COMMAND bash ${PROJECT_SOURCE_DIR}/gen_version.sh >${CMAKE_BINARY_DIR}/.version)

add_custom_target(dist
    COMMAND ${CMAKE_SOURCE_DIR}/dist/make-dist.sh)

# run unit test inside docker
add_custom_target(test
    COMMAND ${CMAKE_SOURCE_DIR}/docker/docker-run.sh ${CMAKE_SOURCE_DIR}/test/run-test.sh unit_test)

# run regression inside docker
add_custom_target(regression
    COMMAND ${CMAKE_SOURCE_DIR}/docker/docker-run.sh ${CMAKE_SOURCE_DIR}/test/run-test.sh regression)

# push docker image to ECR
add_custom_target(docker_push
    COMMAND ${CMAKE_SOURCE_DIR}/docker/docker-push.sh)

# run code coverage
add_custom_target(coverage
    COMMAND lcov --no-external --directory ${PROJECT_SOURCE_DIR} --base-directory ${PROJECT_SOURCE_DIR}/build/ --gcov-tool ${PROJECT_SOURCE_DIR}/llvm-gcov.sh --capture -o mc.info 
    COMMAND genhtml mc.info -o output)
