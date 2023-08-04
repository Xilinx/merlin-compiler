# Copyright (C) 2019 Xilinx, Inc. - All rights reserved.
#
# Choose target FPGA platform & vendor
VENDOR=INTEL
DEVICE=a10gx

# Edit Options per your setup if using AWS F1
#S3_OPT=-s3_bucket=fcs-fpga -s3_dcp_key=dcp -s3_logs_key=log

# Host Code Compilation settings
SRC_DIR=../src
INC_DIR=../include
#COMMON_DIR:=../../common
HOST_SRC_FILES=$(SRC_DIR)/main.cpp
DATA_DIR=../../data
F1_DATA_DIR=./data

AUTODSE=1
# Executable names and arguments
EXE=vectoradd
ACC_EXE=$(EXE)_acc
# Testing mode
EXE_ARGS= 
F1_EXE_ARGS= 

CXX_INC_DIRS=-I. -I$(INC_DIR) 
CXX_FLAGS+= $(CXX_INC_DIRS)
CXX_FLAGS+= -Wall -O3 -lrt
CXX_FLAGS+=-DPLATFORM=\"$(PLATFORM)\" -DKERNEL=\"$(KERNEL_NAME)\" -DKERNEL_NAME=$(KERNEL_NAME) 
# Uncomment the following line to compile the Baseline
CXX_FLAGS+= -DBASELINE=1 -DAUTODSE=1
#CXX_FLAGS+= -g


# Accelerated Kernel settings
KERNEL_NAME=vec_add_kernel
KERNEL_SRC_FILES=$(SRC_DIR)/vec_add_kernel.cpp
KERNEL_INC_DIR=$(CXX_INC_DIRS)

# MerlinCC Options
# Uncomment the following lines to compile with Baseline
CMP_OPT= -d11 -DBASELINE=1 -DAUTODSE=1 
LNK_OPT= -d11 -DBASELINE=1 -DAUTODSE=1 

#common make
MCC_COMMON_DIR=$(MERLIN_COMPILER_HOME)/regression_test/configuration
include $(MCC_COMMON_DIR)/mcc_common.mk

