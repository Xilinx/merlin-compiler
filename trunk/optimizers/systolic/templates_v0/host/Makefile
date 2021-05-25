
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
CXXFLAGS += -g -DLINUX -DALTERA_CL
else
CXXFLAGS += -O3 -DLINUX -DALTERA_CL
endif

$(warning $(CXXFLAGS))

# Compiler
CXX := g++

# Target
TARGET := cnn.out
TARGET_DIR := bin

# Directories
INC_DIRS := inc
LIB_DIRS :=  

# Files
INCS := $(wildcard inc/*.h)
SRCS := $(wildcard src/*.cpp)
LIBS := 

# Make it all!
all : $(TARGET_DIR)/$(TARGET)

# Host executable target.
$(TARGET_DIR)/$(TARGET) : Makefile $(SRCS) $(INCS)
	@[ -d $(TARGET_DIR) ] || mkdir $(TARGET_DIR)
	$(ECHO)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC $(foreach D,$(INC_DIRS),-I$D) \
			$(AOCL_COMPILE_CONFIG) $(SRCS) $(AOCL_LINK_CONFIG) \
			$(foreach D,$(LIB_DIRS),-L$D) \
			$(foreach L,$(LIBS),-l$L) \
			-o $(TARGET_DIR)/$(TARGET)

# Standard make targets
clean :
	$(ECHO)rm -f $(TARGET_DIR)/$(TARGET)

.PHONY : all clean
