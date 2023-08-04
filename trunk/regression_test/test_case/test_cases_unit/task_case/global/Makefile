# Choose target FPGA platform vendor
#VENDOR=XILINX
VENDOR=INTEL

# Host Code Compilation settings
MACH_COMMON_DIR=../../../common
HOST_SRC_FILES=src/vec_add.cpp

# Executable names and arguments
EXE=test_multi_die
ACC_EXE=test_acc_multi_die
EXE_ARGS=

CXX=g++
CXX_INC_DIRS=-I src -I ./ -I $(MACH_COMMON_DIR)
CXX_FLAGS+= $(CXX_INC_DIRS) -Wall -O3
#CXX_FLAGS+= -Wall
#CXX_FLAGS+= -g
ifeq ($(VENDOR),XILINX)

CXX_FLAGS +=-lstdc++ -L$(XILINX_SDX)/lib/lnx64.o -L$(XILINX_XRT)/lib
endif

# Accelerated Kernel settings
KERNEL_NAME=test_kernel
KERNEL_SRC_FILES=src/vec_add.cpp
KERNEL_INC_DIR=$(CXX_INC_DIRS)
CMP_OPT=-d11
LNK_OPT=-d11

MCC_COMMON_DIR=$(MERLIN_COMPILER_HOME)/regression_test/configuration
include $(MCC_COMMON_DIR)/mcc_common.mk
