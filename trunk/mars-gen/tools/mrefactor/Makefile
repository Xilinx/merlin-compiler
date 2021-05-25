CXX = g++

# TODO: This really should be $(CXXFLAGS)
CPPFLAGS ?= \
	-ggdb \
	-std=c++11 \
	-DNDEBUG \
	-O3 \
	-fvisibility-inlines-hidden \
	-ffunction-sections \
	-fdata-sections \
	-fno-common \
	-fno-strict-aliasing \
	-fno-exceptions \
	-fno-rtti \
	-pedantic
LINKFLAGS ?= -Wl,-allow-shlib-undefined -Wl,-O3 -Wl,--gc-sections

INCLUDES ?= ${MERLIN_COMPILER_HOME}/source-opt/lib/clang/include
LIB_DIR ?= ${MERLIN_COMPILER_HOME}/source-opt/lib/clang/lib

BIN_DIR = ${MERLIN_COMPILER_HOME}/build/bin

LIBS ?=\
	-lrt \
	-ldl \
	-lpthread \
	-lm \
	-lpthread \
	-lclangTooling \
	-lclangBasic \
	-lclangASTMatchers \
	-lclangFrontend \
	-lclangAST \
	-lclangParse \
	-lLLVMMCParser \
	-lclangSerialization \
	-lclangSema \
	-lclangEdit \
	-lclangAnalysis \
	-lLLVMBitReader \
	-lLLVMProfileData \
	-lclangDriver \
	-lclangFormat \
	-lclangToolingCore \
	-lclangAST \
	-lclangRewrite \
	-lclangLex \
	-lclangBasic \
	-lLLVMCore \
	-lLLVMBinaryFormat \
	-lLLVMMC \
	-lLLVMOption \
	-lLLVMSupport \
	-lLLVMDemangle \
	-lLLVMSupport

comma := ,

# relative path to $project_root/trunk from $project_root/trunk/bin/unwrapped
REL_MERLIN_COMPILER_HOME ?= ../..

# Copied from set_env/env.sh. We need this primarily to run mcheck wherever we
# want without having to set LD_LIBRARY_PATH.
RPATHS ?=\
	$(REL_MERLIN_COMPILER_HOME)/source-opt/lib/jdk1.8.0_05/jre/lib/amd64/server \
	$(REL_MERLIN_COMPILER_HOME)/source-opt/lib/boost_1_53_0/build/lib \
	$(REL_MERLIN_COMPILER_HOME)/source-opt/lib \
	$(REL_MERLIN_COMPILER_HOME)/source-opt/lib/libstdc++6-4.8.2/lib64 

build: $(BIN_DIR)/mrefactor

$(BIN_DIR)/mrefactor: mrefactor.cpp 
	$(CXX) $(patsubst %,-I%,$(INCLUDES)) $(CPPFLAGS) $(LINKFLAGS) $^ \
		-o $@ $(patsubst %,-Wl$(comma)-L%,$(LIB_DIR)) $(LIBS) \
		$(patsubst %,-Wl$(comma)-rpath='$$ORIGIN/%',$(RPATHS))
