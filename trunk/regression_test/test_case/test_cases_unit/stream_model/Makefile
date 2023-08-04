unit_test: diff
gen_ref: update
init:
	rm -rf rose_* err.log
	touch err.log
run: init
	-mars_opt matmul_kernel_port_gen.cpp -e c -p test_stream_ir -a test_port_gen 2>tmp.log || cat tmp.log &>>err.log
	-mars_opt matmul_kernel.cpp -e c -p test_stream_ir -a test_spawn 2>tmp.log || cat tmp.log &>> err.log
	-make -C unit_test
diff:run 
	-diff -du rose_matmul_kernel_port_gen.cpp matmul_kernel_port_gen.out.ref &>>err.log
	-diff -du rose_matmul_kernel.cpp matmul_kernel.out.ref &>>err.log
	-make diff -C unit_test
update:run 
	-cp  rose_matmul_kernel_port_gen.cpp matmul_kernel_port_gen.out.ref 
	-cp  rose_matmul_kernel.cpp matmul_kernel.out.ref
	-make update -C unit_test
