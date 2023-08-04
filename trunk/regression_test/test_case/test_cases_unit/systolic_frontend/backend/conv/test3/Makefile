
TARGET := cnn_sim.out

SRCS := main.cpp
OBJS := main.o

CXX_FLAGS := -g -Wall -I.
LD_FLAGS := -lkernel -L.

sim: merlincc $(TARGET)
	export LD_LIBRARY_PATH=./:$$LD_LIBRARY_PATH; \
	env CL_CONTEXT_EMULATOR_DEVICE_INTELFPGA=1 ./$(TARGET) kernel_top.aocx

merlincc:
	make -C src merlincc

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LD_FLAGS)

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXX_FLAGS)

clean:
	make -C src clean
	rm -rf *.o $(TARGET)
