set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_FLAGS "-std=c++11 -fvisibility-inlines-hidden -ffunction-sections -fdata-sections -fno-common -fno-strict-aliasing -fno-exceptions -fno-rtti -pedantic -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
 
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-allow-shlib-undefined -Wl,--as-needed -Wl,--gc-sections")

include_directories(${CLANG_ROOT_DIR}/include)
link_directories(${CLANG_ROOT_DIR}/lib)

add_executable(mcheck mcheck.cpp mdiag.cpp)

target_link_libraries(mcheck
	clangTooling
	clangBasic
	clangASTMatchers
	clangFrontend
	clangAST
	clangParse
	LLVMMCParser
	clangSerialization
	clangSema
	clangEdit
	clangAnalysis
	LLVMBitReader
	LLVMProfileData
	clangDriver
	clangFormat
	clangToolingCore
	clangAST
	clangRewrite
	clangLex
	clangBasic
	LLVMCore
	LLVMBinaryFormat
	LLVMMC
	LLVMOption
	LLVMSupport
	LLVMDemangle
	LLVMSupport
	${Boost_LIBRARIES}
	${CMAKE_DL_LIBS} 
	${CMAKE_THREAD_LIBS_INIT}
	m
  z
  tinfo)

