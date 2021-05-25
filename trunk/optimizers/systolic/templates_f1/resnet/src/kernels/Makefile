
ifndef XILINX_SDX
$(error Environment variable XILINX_SDX is required and should point to SDAccel install area)
endif

SDA_FLOW = cpu_emu


KERNEL_SRCS = cnn_top.cpp
#KERNEL_SRCS = systolic_intel.cpp
KERNEL_NAME = cnn_top

XOBJECTS := $(KERNEL_SRCS:.cpp=.xo)
$(warning $(XOBJECTS))

CLCC_CFLAGS = -I./ -I$(SYSTOLIC_ROOT)/src/host -I$(SYSTOLIC_ROOT)/inc

#set target device for XCLBIN
XDEVICE_REPO_PATH=

XDEVICE=xilinx_aws-vu9p-f1_dynamic_5_0
#XDEVICE=xilinx:aws-vu9p-f1:4ddr-xpr-2pr:4.0

#USE_4DDR=1
#XDEVICE=xilinx:adm-pcie-ku3:2ddr-xpr:4.0
#XDEVICE=xilinx:xil-accel-rd-ku115:4ddr-xpr:4.0
#XDEVICE=xilinx:adm-pcie-7v3:1ddr:3.0

#ifeq ($(USE_4DDR),1)
#    HOST_CFLAGS += -DUSE_4DDR 
#    KERNEL_DEFS += -DUSE_4DDR
#    CLCC_OPT = --max_memory_ports all \
#                --xp misc:map_connect=add.kernel.bandwidth_1.M_AXI_GMEM0.core.OCL_REGION_0.M00_AXI \
#                --xp misc:map_connect=add.kernel.bandwidth_1.M_AXI_GMEM1.core.OCL_REGION_0.M01_AXI \
#                --xp misc:map_connect=add.kernel.bandwidth_1.M_AXI_GMEM2.core.OCL_REGION_0.M02_AXI \
#                --xp misc:map_connect=add.kernel.bandwidth_1.M_AXI_GMEM3.core.OCL_REGION_0.M03_AXI
#else ifeq ($(XDEVICE),xilinx:adm-pcie-ku3:2ddr-xpr:4.0)
#    HOST_CFLAGS += -DUSE_2DDR
#    CLCC_OPT = --max_memory_ports all \
#                --xp misc:map_connect=add.kernel.bandwidth_1.M_AXI_GMEM0.core.OCL_REGION_0.M00_AXI \
#                --xp misc:map_connect=add.kernel.bandwidth_1.M_AXI_GMEM1.core.OCL_REGION_0.M01_AXI
#else 
#
#endif

#CLCC_OPT += --kernel_frequency 90

#CLCC_OPT += --max_memory_ports all \
#		   --xp param:solution.kernel_compiler_margin=0.5

KEEP_TEMP=1
KERNEL_DEBUG=0

XCLBIN_NAME=cnn

ifeq (${SDA_FLOW},cpu_emu)
    CLCC_OPT += -t sw_emu
    XCLBIN = ${XCLBIN_NAME}_cpu_emu.xclbin
else ifeq (${SDA_FLOW},hw_emu)
    CLCC_OPT += -t hw_emu
    XCLBIN = ${XCLBIN_NAME}_hw_emu.xclbin
else ifeq (${SDA_FLOW},hw)
    XCLBIN = ${XCLBIN_NAME}_hw.xclbin
    CLCC_OPT += -t hw
else
    $(error unsupported SDA_FLOW value $(SDA_FLOW))
endif

#HOST_ARGS = ${XCLBIN}


#supported flow: cpu_emu, hw_emu, hw
CLCC = xocc

ifeq ($(XDEVICE_REPO_PATH),)
#no device repo path set. do nothing
    DEVICE_REPO_OPT = 
else
    DEVICE_REPO_OPT = --xp prop:solution.device_repo_paths=${XDEVICE_REPO_PATH} 
endif


#CLCC_OPT += $(CLCC_OPT_LEVEL) ${DEVICE_REPO_OPT} --xdevice ${XDEVICE} -o ${XCLBIN} ${KERNEL_DEFS} ${KERNEL_INCS}
#CLCC_OPT += $(CLCC_OPT_LEVEL) ${DEVICE_REPO_OPT} --xdevice ${XDEVICE}
CLCC_OPT += $(CLCC_OPT_LEVEL) ${DEVICE_REPO_OPT} --platform ${XDEVICE}

ifeq (${KEEP_TEMP},1)
    CLCC_OPT += -s
endif

ifeq (${KERNEL_DEBUG},1)
    CLCC_OPT += -g
endif

CLCC_OPT += --kernel ${KERNEL_NAME}
#CLCC_OPT += --report estimate
CLCC_OPT += --kernel_frequency 100 --xp prop:solution.kernel_compiler_margin=0

#CLCC_LFLAGS += --xp param:compiler.fsanitize=address,memory


#.PHONY: all
#
#all: run
#
#host: ${HOST_EXE_DIR}/${HOST_EXE}
#
#xbin_cpu_em:
#	make SDA_FLOW=cpu_emu xbin -f Makefile
#
#xbin_hw_em:
#	make SDA_FLOW=hw_emu xbin -f Makefile
#
#xbin_hw :
#	make SDA_FLOW=hw xbin -f Makefile
#
#xbin: ${XCLBIN}
#
#run_cpu_em: 
#	make SDA_FLOW=cpu_emu run_em -f Makefile
#
#run_hw_em: 
#	make SDA_FLOW=hw_emu run_em -f Makefile
#
#run_hw : 
#	make SDA_FLOW=hw run_hw_int -f Makefile
#
#run_em: xconfig host xbin
##run_em: host xbin
#	XCL_EMULATION_MODE=true ${HOST_EXE_DIR}/${HOST_EXE} ${HOST_ARGS}
#
#run_hw_int : host xbin_hw
#	source ${BOARD_SETUP_FILE};${HOST_EXE_DIR}/${HOST_EXE} ${HOST_ARGS}

all: $(XCLBIN)

${XCLBIN}: $(XOBJECTS)
	${CLCC} -l $(CLCC_LFLAGS) ${CLCC_OPT} -o $@ $^

cnn_top.xo: cnn_top.cpp
	$(CLCC) $(CLCC_CFLAGS) $(CLCC_OPT) -o $@ -c $<

#estimate:
#	${CLCC} -c -t hw_emu --xdevice ${XDEVICE} --report estimate ${KERNEL_SRCS} -k cnn_top

xconfig : emconfig.json

emconfig.json :
	emconfigutil --platform ${XDEVICE} ${DEVICE_REPO_OPT} --od .
#	emconfigutil --xdevice ${XDEVICE} ${DEVICE_REPO_OPT} --od .


clean:
	${RM} -rf ${XCLBIN} emconfig.json _xocc_${XCLBIN_NAME}_*.dir .Xil

cleanall: clean
	${RM} -rf $(XOBJECTS) *.xclbin sdaccel_profile_summary.* _xocc_compile* _xocc_link* _xocc_krnl* TempConfig


#help:
#	@echo "Compile and run CPU emulation using default xilinx:adm-pcie-7v3:1ddr:3.0 DSA"
#	@echo "make -f Makefile run_cpu_em"
#	@echo ""
#	@echo "Compile and run hardware emulation using default xilinx:adm-pcie-7v3:1ddr:3.0 DSA"
#	@echo "make -f Makefile run_hw_em"
#	@echo ""
#	@echo "Compile host executable only"
#	@echo "make -f Makefile host"
#	@echo ""
#	@echo "Compile XCLBIN file for system run only"
#	@echo "make -f Makefile xbin_hw"
#	@echo ""
#	@echo "Compile and run CPU emulation using xilinx:tul-pcie3-ku115:2ddr:3.0 DSA"
#	@echo "make -f Makefile XDEVICE=xilinx:tul-pcie3-ku115:2ddr:3.0 run_cpu_em"
#	@echo ""
#	@echo "Clean working diretory"
#	@echo "make -f Makefile clean"
#	@echo ""
#	@echo "Super clean working directory"
#	@echo "make -f Makefile cleanall"
