
ifeq ($(VERBOSE),1)
	ECHO :=
else
	ECHO := @
endif

CC = g++

HOST_SRCS = $(wildcard $(SYSTOLIC_ROOT)/src/host/*.cpp)
OBJECTS := $(HOST_SRCS:.cpp=.o)

$(warning $(HOST_SRCS))

HOST_EXE_DIR := bin
HOST_EXE := cnn.out 

HOST_CFLAGS = -g -Wall -DFPGA_DEVICE -DC_KERNEL
HOST_LFLAGS = 

#XDEVICE=xilinx:aws-vu9p-f1:4ddr-xpr-2pr:4.0
#XDEVICE=xilinx:adm-pcie-ku3:2ddr-xpr:4.0
#XDEVICE=xilinx:xil-accel-rd-ku115:4ddr-xpr:4.0

XDEVICE=xilinx_aws-vu9p-f1_dynamic_5_0

HOST_CFLAGS+=-DTARGET_DEVICE=\"${XDEVICE}\"

#HOST_LFLAGS += ${XILINX_SDACCEL}/lib/lnx64.o/libstdc++.so.6
HOST_CFLAGS += -I${XILINX_SDX}/runtime/include/1_2
HOST_CFLAGS += -I./ -I$(SYSTOLIC_ROOT)/inc

HOST_LFLAGS += -L${XILINX_SDX}/runtime/lib/x86_64 -lxilinxopencl -lstdc++

${HOST_EXE_DIR}/${HOST_EXE} : ${OBJECTS}
	@[ -d $(HOST_EXE_DIR) ] || mkdir $(HOST_EXE_DIR)
	${CC} ${HOST_LFLAGS} ${OBJECTS} -o $@ 

%.o: %.cpp
	${CC} ${HOST_CFLAGS} -c $< -o $@

clean:
	rm -rf $(OBJECTS) $(HOST_EXE)
