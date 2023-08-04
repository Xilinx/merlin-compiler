
ifeq ($(VERBOSE),1)
ECHO :=
else
ECHO := @
endif

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

# Compilation flags
ifeq ($(DEBUG),1)
CXXFLAGS += -g
else
CXXFLAGS += -O2
endif

#CXXFLAGS += -fpermissive
CXXFLAGS += -lstdc++ -lm -std=c99
#CXXFLAGS += -lstdc++ -lm

# Compiler
#CXX := g++
CXX := gcc

# Target
TARGET := host_top
TARGET_DIR := bin

# Directories
INC_DIRS :=   #../common/inc
LIB_DIRS :=

# Files
INCS := $(wildcard )
#SRCS := $(wildcard host/src/*.cpp ../common/src/AOCLUtils/*.cpp)
SRCS := $(wildcard *.c *.cpp)
LIBS := acl_emulator_kernel_rt rt


all :
	@echo 'make [host|sim|estimate|bit]'

host : $(TARGET_DIR)/$(TARGET)
	@echo ''
#mars_perl -altera altera_run_flow.pl $(platform) host
sim  :
	mars_perl -altera altera_run_flow.pl $(platform) sim $(merlincc) "$(library_name)" "$(library_path)"
sim_gdb  :
	mars_perl -altera altera_run_flow.pl $(platform) sim_gdb $(merlincc) "$(library_name)" "$(library_path)"
estimate :
	mars_perl -altera altera_run_flow.pl $(platform) estimate
hls :
	mars_perl -altera altera_run_flow.pl $(platform) estimate
bit:
	mars_perl -altera altera_run_flow.pl $(platform) bit
profile_bit:
	mars_perl -altera altera_run_flow.pl $(platform) profile_bit
test:
	mars_perl -altera altera_run_flow.pl $(platform) test
profile_test:
	mars_perl -altera altera_run_flow.pl $(platform) profile_test

# Host executable target.
$(TARGET_DIR)/$(TARGET) : Makefile $(SRCS) $(INCS)
	@[ -d $(TARGET_DIR) ] || mkdir $(TARGET_DIR)
	$(ECHO)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC $(INC_DIRS) \
			$(AOCL_COMPILE_CONFIG) $(SRCS) $(AOCL_LINK_CONFIG) \
			$(foreach D,$(LIB_DIRS),-L$D) \
			$(foreach L,$(LIBS),-l$L) \
			-DALTERA_KERNEL \
			-o $(TARGET_DIR)/$(TARGET) \
			-L./ \
			-Wl,-rpath=./ \
			-lkernel

# Standard make targets
clean :
	$(ECHO)rm -f $(TARGET_DIR)/$(TARGET)

.PHONY : all clean
