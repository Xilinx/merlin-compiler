# Copyright (C) 2017 Xilinx, Inc. - All rights reserved.
#
# Choose target FPGA platform & vendor
VENDOR=XILINX
#DEVICE=xilinx_vcu1525_dynamic_5_0
#DEVICE=xilinx:adm-pcie-ku3:2ddr:3.3
#DEVICE=xilinx:adm-pcie-ku3:2ddr-xpr:4.0
#DEVICE=xilinx:aws-vu9p-f1:4ddr-xpr-2pr:4.0
#VENDOR=INTEL
#DEVICE=a10gx

# Edit Options per your setup if using AWS F1
#S3_OPT=-s3_bucket=fcs-fpga -s3_dcp_key=dcp -s3_logs_key=log

# Host Code Compilation settings
HOST_SRC_FILES= ../src/main.c

# Executable names and arguments
EXE=acoshll
ACC_EXE=acoshll_hw
# Testing mode
EXE_ARGS= 

CXX_INC_DIRS=-I ./ -I ../src/
CXX_FLAGS+= $(CXX_INC_DIRS)
CXX_FLAGS+= -Wall -O3 -lrt -lm -Wno-unknown-pragmas -std=c++0x
#CXX_FLAGS+= -g -lm

# Accelerated Kernel settings
KERNEL_NAME=acoshll
KERNEL_SRC_FILES=../src/acoshll.c
KERNEL_INC_DIR=$(CXX_INC_DIRS)

# MerlinCC Options
CMP_OPT= -d11 --attribute memory_coalescing=off --attribute memory_burst=off
LNK_OPT= -g -d11 

MCC_COMMON_DIR=../../
include $(MCC_COMMON_DIR)/mcc_common.mk

