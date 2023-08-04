
TARGET := cnn_sim.out

CASE := test.cpp

SRCS := main.cpp
OBJS := main.o

CXX_FLAGS := -g -Wall -I.
LD_FLAGS := -lkernel -L.

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LD_FLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXX_FLAGS)

build_src:
	merlincc $(CASE) -p=aocl::a10gx -c -d11 --attribute systolic_array=on

build_hw: build_src
	merlincc kernel_top.mco -d11 -p=aocl::a10gx

build_sim: build_src
	merlincc kernel_top.mco -d11 -march=sw_emu

run_sim: build_sim $(TARGET)
	export LD_LIBRARY_PATH=./:$$LD_LIBRARY_PATH; \
	env CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA=1 ./$(TARGET) kernel_top.aocx

run_hw: $(TARGET)
	export LD_LIBRARY_PATH=./:$$LD_LIBRARY_PATH; \
	./$(TARGET) kernel_top.aocx

clean:
	rm -rf *.o $(TARGET)

distclean: clean
	rm -rf kernel_top.* *.log *.so profile.mon __merlinkernel_top.h .Mer .merlin_prj
