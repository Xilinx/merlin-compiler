# Find Clang
#
# It defines the following variables
# CLANG_FOUND        - True if Clang found.
# CLANG_INCLUDE_DIRS - where to find Clang include files
# CLANG_LIBRARIES    - list of clang libs
# CLANG_CXXFLAGS     - COMPILE_FLAGS for Clang

if(NOT LLVM_INCLUDE_DIRS OR NOT LLVM_LIBRARY_DIRS)
   message(FATAL_ERROR "No LLVM and Clang support requires LLVM")
else()
  macro(FIND_AND_ADD_CLANG_LIB _libname_)
    find_library(CLANG_${_libname_}_LIB ${_libname_} 
        HINTS ${LLVM_LIBRARY_DIRS}
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_DEFAULT_PATH)
    if(CLANG_${_libname_}_LIB)
      set(CLANG_LIBRARIES ${CLANG_LIBRARIES} ${CLANG_${_libname_}_LIB})
    else()
      message(FATAL_ERROR "${_libname_} not found")
    endif()
  endmacro()

  # Clang shared library provides just the limited C interface, so it
  # can not be used.  We look for the static libraries.
  FIND_AND_ADD_CLANG_LIB(clangAST)
  FIND_AND_ADD_CLANG_LIB(clangAnalysis)
  FIND_AND_ADD_CLANG_LIB(clangBasic)
  FIND_AND_ADD_CLANG_LIB(clangDriver)
  FIND_AND_ADD_CLANG_LIB(clangFrontend)
  FIND_AND_ADD_CLANG_LIB(clangLex)
  FIND_AND_ADD_CLANG_LIB(clangParse)
  FIND_AND_ADD_CLANG_LIB(clangRewrite)
  FIND_AND_ADD_CLANG_LIB(clangSema)
  FIND_AND_ADD_CLANG_LIB(clangSerialization)
  FIND_AND_ADD_CLANG_LIB(clangTooling)
  FIND_AND_ADD_CLANG_LIB(clangASTMatchers)
  FIND_AND_ADD_CLANG_LIB(clangEdit)
  FIND_AND_ADD_CLANG_LIB(clangFormat)
  FIND_AND_ADD_CLANG_LIB(clangToolingCore)

  find_path(CLANG_INCLUDE_DIRS clang/Basic/Version.h HINTS "${LLVM_ROOT_DIR}/include")

  if(NOT CLANG_INCLUDE_DIRS)
    message(FATAL_ERROR "Could not find clang include directories")
  endif()

  if(NOT CLANG_LIBRARIES AND NOT CLANG_INCLUDE_DIRS AND CLANG_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find Clang")
  endif()
endif()
find_package_handle_standard_args(Clang
    REQUIRED_VARS LLVM_ROOT_DIR LLVM_HOST_TARGET
    VERSION_VAR LLVM_VERSION_STRING)
