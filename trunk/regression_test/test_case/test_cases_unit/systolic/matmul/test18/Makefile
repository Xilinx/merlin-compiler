
TARGET := cnn_sim.out

SRCS := main.cpp
OBJS := main.o

CXX_FLAGS := -g -Wall -I.
LD_FLAGS := -lkernel -L.

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LD_FLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXX_FLAGS)

merlincc:
	merlincc test.cpp -p=aocl::a10gx -c -d11 --attribute systolic_array=on
	merlincc kernel_top.mco -d11 -march=sw_emu

run: merlincc $(TARGET)
	export LD_LIBRARY_PATH=./:$$LD_LIBRARY_PATH; \
	env CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA=1 ./$(TARGET) kernel_top.aocx

clean:
	rm -rf *.o $(TARGET)

distclean: clean
	rm -rf kernel_top.* *.log *.so profile.mon __merlinkernel_top.h .Mer .merlin_prj
