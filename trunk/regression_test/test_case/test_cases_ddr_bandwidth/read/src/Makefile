all: clean
	gcc -O3 -c -I/curr/software/Xilinx/Vivado_HLS/2015.3/include/ mem_test.c
	gcc -O3 -c -I/curr/software/Xilinx/Vivado_HLS/2015.3/include/ mem_test_kernel.cpp
	g++ -O3 mem_test.o mem_test_kernel.o
clean:
	rm -rf *.o a.out c_out.dat
