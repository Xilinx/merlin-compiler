#DIE_OPTION=
#BANK_OPTION=
SHELL = /bin/bash

RUNTIME_FLAGS := $(XILINX_SDX)/runtime/include/1_2
RUNTIME_LINK := $(XILINX_SDX)/runtime/lib/x86_64

ifdef XILINX_XRT
FILE=$(XILINX_XRT)/include
ifeq ($(FILE), $(wildcard $(FILE)))
RUNTIME_FLAGS := $(FILE)
endif
FILE=$(XILINX_XRT)/lib
ifeq ($(FILE), $(wildcard $(FILE)))
RUNTIME_LINK := $(FILE)
endif
endif

ifdef XILINX_VIVADO
VIVADO_INCLUDE=$(XILINX_VIVADO)/include
endif

ifeq ($(VERBOSE),1)
ECHO :=
else
ECHO := @
endif
ECHO := @

HOST_EXE_DIR = ./
HOST_EXE = host_top

SRCS := $(wildcard *.c *.cpp)
XFLAGS= -fPIC -DHLS_NO_XIL_FPO_LIB \
		-I$(RUNTIME_FLAGS) \
		-I$(VIVADO_INCLUDE) \
		-I$(XILINX_SDX)/Vivado_HLS/include \
		-I$(XILINX_SDX)/include
LINK=   -L$(RUNTIME_LINK) \
	    -lxilinxopencl\
		-lpthread -lm -ldl -lrt -lstdc++ -std=c99 -fgnu89-inline

ESTIMATE =
XCLBIN_NAME=kernel_top
XCLBIN = ${XCLBIN_NAME}.xclbin

#supported flow: cpu_sim, hw_sim, hw
CC = gcc
CLCC = xocc

CMD = mars_perl
RUN_SCRIPT = $(MERLIN_COMPILER_HOME)/mars-gen/scripts/sdaccel/sdaccel_run_flow.pl

#CLCC_OPT += --kernel ${KERNEL_NAME}

.PHONY: all

all: run

preprocess:
	@mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/preprocess.pl

host:
	$(ECHO) $(CC) -o host_top $(SRCS) $(XFLAGS) $(CFLAGS) $(LINK)  \
	   		$(MERLIN_INCLUDE) \
			-L./ \
			-Wl,-rpath=./ \
			-lkernel

lib_gen:
	@rm -rf bin;
	@mkdir bin;
	$(ECHO) $(CC) -O -shared -fPIC -o bin/libkernel.so \
			$(SRCS) $(XFLAGS) $(CFLAGS) $(LINK)

kernel_gen:
	@mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/kernel_gen.pl

sim_config: .IGNORE
	@$(CMD) $(RUN_SCRIPT) sim_config

hw_sim_config: .IGNORE
	@$(CMD) $(RUN_SCRIPT) hw_sim_config

csim: .IGNORE
	@$(CMD) $(RUN_SCRIPT) csim

cosim: .IGNORE
	@$(CMD) $(RUN_SCRIPT) cosim

exec_sim:
	@$(CMD) $(RUN_SCRIPT) exec_sim

hls:
	@$(CMD) $(RUN_SCRIPT) hls

impl:
	@$(CMD) $(RUN_SCRIPT) impl \"${BANK_OPTION}\" \"${DIE_OPTION}\"

exec:
	cp ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/run.pl fpga_run/
	cp ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/analyze_profile.py fpga_run/
	mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/merlin_flow/remote_exec.pl 'perl run.pl $(HOST_ARGS)' -server $(SERVER) -copy -ldir ./fpga_run;
	cd fpga_run; mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/sdaccel/profile.pl sdaccel_profile_summary.csv

.IGNORE :
	@emconfigutil --platform ${DEVICE} --od . >emconfig.log 2>&1

clean:
	@${RM} -rf ${HOST_EXE} ${XCLBIN} emconfig.json _xocc_${XCLBIN_NAME}_*.dir .Xil

cleanall: clean
	@${RM} -rf *.xclbin sdaccel_profile_summary.* _xocc_compile* _xocc_link* _xocc_krnl* TempConfig

