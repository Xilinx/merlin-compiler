set(CMAKE_C_COMPILER "gcc") # gcc should be the version which compiles the clang6.0
set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_FLAGS "-std=c++11 -fvisibility-inlines-hidden -ffunction-sections -fdata-sections -fno-common -fno-strict-aliasing -fno-exceptions -fno-rtti -pedantic -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
 
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-allow-shlib-undefined -Wl,--as-needed -Wl,--gc-sections")
set(CMAKE_PREFIX_PATH ${CLANG_ROOT_DIR}/lib)
include_directories(${CLANG_ROOT_DIR}/include)
link_directories(${CLANG_ROOT_DIR}/lib)

add_executable(mcheck mcheck.cpp mdiag.cpp)

target_link_libraries(mcheck
  ${CLANG_ROOT_DIR}/lib/libclangTooling.a
  ${CLANG_ROOT_DIR}/lib/libclangBasic.a
  ${CLANG_ROOT_DIR}/lib/libclangASTMatchers.a
  ${CLANG_ROOT_DIR}/lib/libclangFrontend.a
  ${CLANG_ROOT_DIR}/lib/libclangAST.a
  ${CLANG_ROOT_DIR}/lib/libclangParse.a
  ${CLANG_ROOT_DIR}/lib/libLLVMMCParser.a
  ${CLANG_ROOT_DIR}/lib/libclangSerialization.a
  ${CLANG_ROOT_DIR}/lib/libclangSema.a
  ${CLANG_ROOT_DIR}/lib/libclangEdit.a
  ${CLANG_ROOT_DIR}/lib/libclangAnalysis.a
  ${CLANG_ROOT_DIR}/lib/libLLVMBitReader.a
  ${CLANG_ROOT_DIR}/lib/libLLVMProfileData.a
  ${CLANG_ROOT_DIR}/lib/libclangDriver.a
  ${CLANG_ROOT_DIR}/lib/libclangFormat.a
  ${CLANG_ROOT_DIR}/lib/libclangToolingCore.a
  ${CLANG_ROOT_DIR}/lib/libclangAST.a
  ${CLANG_ROOT_DIR}/lib/libclangRewrite.a
  ${CLANG_ROOT_DIR}/lib/libclangLex.a
  ${CLANG_ROOT_DIR}/lib/libclangBasic.a
  ${CLANG_ROOT_DIR}/lib/libLLVMCore.a
  ${CLANG_ROOT_DIR}/lib/libLLVMBinaryFormat.a
  ${CLANG_ROOT_DIR}/lib/libLLVMMC.a
  ${CLANG_ROOT_DIR}/lib/libLLVMOption.a
  ${CLANG_ROOT_DIR}/lib/libLLVMSupport.a
  ${CLANG_ROOT_DIR}/lib/libLLVMDemangle.a
  ${CLANG_ROOT_DIR}/lib/libLLVMSupport.a
  ${Boost_LIBRARIES}
  ${CMAKE_DL_LIBS} 
  ${CMAKE_THREAD_LIBS_INIT}
  m
  z
  tinfo)
