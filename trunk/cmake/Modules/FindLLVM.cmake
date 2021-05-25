# - Find LLVM headers and libraries.
# This module locates LLVM and adapts the llvm-config output for use with
# CMake.
#
# A given list of COMPONENTS is passed to llvm-config.
#
# The following variables are defined:
#  LLVM_CXXFLAGS       - C++ compiler flags for files that include LLVM headers.
#  LLVM_HOST_TARGET    - Target triple used to configure LLVM.
#  LLVM_INCLUDE_DIRS   - Directory containing LLVM include files.
#  LLVM_LDFLAGS        - Linker flags to add when linking against LLVM
#                        (includes -LLLVM_LIBRARY_DIRS).
#  LLVM_LIBRARIES      - Full paths to the library files to link against.
#  LLVM_LIBRARY_DIRS   - Directory containing LLVM libraries.
#  LLVM_ROOT_DIR       - The root directory of the LLVM installation.
#                        llvm-config is searched for in ${LLVM_ROOT_DIR}/bin.
#  LLVM_VERSION_MAJOR  - Major version of LLVM.
#  LLVM_VERSION_MINOR  - Minor version of LLVM.
#  LLVM_VERSION_STRING - Full LLVM version string (e.g. 2.9).

# Try suffixed versions to pick up the newest LLVM install available on Debian
# derivatives.
# We also want an user-specified LLVM_ROOT_DIR to take precedence over the
# system default locations such as /usr/local/bin. Executing find_program()
# multiples times is the approach recommended in the docs.
set(llvm_config_names llvm-config)
find_program(LLVM_CONFIG
    NAMES ${llvm_config_names}
    PATHS ${LLVM_ROOT_DIR}/bin NO_DEFAULT_PATH
    DOC "Path to llvm-config tool.")
find_program(LLVM_CONFIG NAMES ${llvm_config_names})

macro(llvm_set var flag)
    execute_process(
        COMMAND ${LLVM_CONFIG} --${flag}
        OUTPUT_VARIABLE LLVM_${var}
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  file(TO_CMAKE_PATH "${LLVM_${var}}" LLVM_${var})
endmacro()

macro(llvm_set_libs var flag prefix)
    execute_process(
        COMMAND ${LLVM_CONFIG} --${flag} ${LLVM_FIND_COMPONENTS}
        OUTPUT_VARIABLE tmplibs
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    file(TO_CMAKE_PATH "${tmplibs}" tmplibs)
    string(REGEX REPLACE "([$^.[|*+?()]|])" "\\\\\\1" pattern "${prefix}/")
    string(REGEX MATCHALL "${pattern}[^ ]+" LLVM_${var} ${tmplibs})
endmacro()

llvm_set(VERSION_STRING version)
llvm_set(CXXFLAGS cxxflags)
llvm_set(HOST_TARGET host-target)
llvm_set(INCLUDE_DIRS includedir true)
llvm_set(ROOT_DIR prefix true)

llvm_set(LDFLAGS ldflags)
llvm_set(LIBRARY_DIRS libdir true)
llvm_set_libs(LIBRARIES libfiles "${LLVM_LIBRARY_DIRS}")

string(REGEX REPLACE "([0-9]+).*" "\\1" LLVM_VERSION_MAJOR "${LLVM_VERSION_STRING}" )
string(REGEX REPLACE "[0-9]+\\.([0-9]+).*[A-Za-z]*" "\\1" LLVM_VERSION_MINOR "${LLVM_VERSION_STRING}" )

string(STRIP "${LLVM_LIBRARY_DIRS}" LLVM_LIBRARY_DIRS)
string(STRIP "${LLVM_LIBRARIES}" LLVM_LIBRARIES)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(LLVM
    REQUIRED_VARS LLVM_ROOT_DIR LLVM_HOST_TARGET
    VERSION_VAR LLVM_VERSION_STRING)
