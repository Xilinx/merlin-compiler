
ifeq ($(DEBUG),1)
ECHO :=
else
ECHO := @
endif
ECHO := @

# Where is the Altera SDK for OpenCL software?
ifeq ($(wildcard $(ALTERAOCLSDKROOT)),)
$(error Set ALTERAOCLSDKROOT to the root directory of the Altera SDK for OpenCL software installation)
endif
ifeq ($(wildcard $(ALTERAOCLSDKROOT)/host/include/CL/opencl.h),)
$(error Set ALTERAOCLSDKROOT to the root directory of the Altera SDK for OpenCL software installation.)
endif

# OpenCL compile and link flags.
AOCL_COMPILE_CONFIG := $(shell aocl compile-config )
AOCL_LINK_CONFIG := $(shell aocl link-config )
TEMP_CONFIG = $(shell echo "$(AOCL_LINK_CONFIG)" | sed "s/-lOpenCL//")

# Compilation flags
ifeq ($(DEBUG),1)
CXXFLAGS += -g
else
CXXFLAGS += -O2
endif

#CXXFLAGS += -fpermissive
CXXFLAGS += -std=c99 -lstdc++ -lm

# Compiler
#CXX := g++
CXX := gcc

# Target
TARGET := host_top
TARGET_DIR := bin
CMD = mars_perl
SCRIPT = $(MERLIN_COMPILER_HOME)/mars-gen/scripts/aocl/altera_run_flow.pl

# Directories
INC_DIRS :=  #../common/inc
LIB_DIRS :=

# Files
INCS := $(wildcard )
#SRCS := $(wildcard host/src/*.cpp ../common/src/AOCLUtils/*.cpp)
SRCS := $(wildcard *.c  *.cpp)
LIBS := acl_emulator_kernel_rt rt

all :
	@echo 'make [host|sim|estimate|bit]'

preprocess:
	@mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/aocl/preprocess.pl
kernel_gen:
	@mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/aocl/kernel_gen.pl
lib_gen:
	@rm -rf bin;
	@mkdir bin;
	$(ECHO)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -shared -fPIC $(foreach D,$(INC_DIRS),-I$D) \
			$(AOCL_COMPILE_CONFIG) $(SRCS) $(TEMP_CONFIG) \
			$(foreach D,$(LIB_DIRS),-L$D) \
			$(foreach L,$(LIBS),-l$L) \
			$(MERLIN_INCLUDE) \
			$(MERLIN_LIB_FLAG) \
			-DALTERA_KERNEL \
			-o bin/libkernel.so
host :
	$(ECHO)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC $(foreach D,$(INC_DIRS),-I$D) \
			$(AOCL_COMPILE_CONFIG) $(SRCS) $(AOCL_LINK_CONFIG) \
			$(foreach D,$(LIB_DIRS),-L$D) \
			$(foreach L,$(LIBS),-l$L) \
			-DALTERA_KERNEL \
			-o $(TARGET) \
			-L./ \
			-Wl,-rpath=./ \
			-lkernel

sim_config:
	$(ECHO)$(CMD) $(SCRIPT) sim_config
exec_sim:
	$(ECHO)$(CMD) $(SCRIPT) exec_sim

hw_sim:
	echo "ERROR : AOCL do not support hardware simulation\n"
hw_sim_config: hw_sim

estimate:
	$(ECHO)$(CMD) $(SCRIPT) estimate
hls: estimate

bit: impl
impl:
	$(ECHO)$(CMD) $(SCRIPT) impl

exec:
	cp ${MERLIN_COMPILER_HOME}/mars-gen/scripts/aocl/run.pl fpga_run/
	cp ${MERLIN_COMPILER_HOME}/mars-gen/scripts/aocl/analyze_profile.pl fpga_run/
	mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/merlin_flow/remote_exec.pl 'perl run.pl $(HOST_ARGS)' -server $(SERVER) -copy -ldir ./fpga_run;
	cd fpga_run; mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/aocl/analyze_profile.pl
	cd fpga_run; mars_perl ${MERLIN_COMPILER_HOME}/mars-gen/scripts/aocl/analyze_profile.pl > profile.log

clean :
	$(ECHO)rm -f $(TARGET_DIR)/$(TARGET)

.PHONY : all clean
