


gen_ref: update_ref
unit_test: diff
run: clean
	-rm -rf err.log
	-mars_opt matmul/matmul_kernel.cpp matmul/matmul.cpp -I matmul -e c -p memory_reduce 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt matmul_ivdep/matmul_ivdep_kernel.cpp matmul_ivdep/matmul.cpp -I matmul_ivdep/ -e c -p memory_reduce 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt flann/vec_add_kernel.cpp flann/vec_add.cpp -e c -p memory_reduce 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt basic/test.c -e c -p memory_reduce 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt cnn/cnn_kernel.cpp -e c -p memory_reduce 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt cnn_2/__merlinkernel_cnn_kernel.c -e c -p memory_reduce 2>tmp.log || (cat tmp.log >> err.log)



update_ref:run 
	cp -f rose_matmul_kernel.cpp test_rule/rose_matmul_kernel.ref
	cp -f rose_matmul_ivdep_kernel.cpp test_rule/rose_matmul_ivdep_kernel.ref
	cp -f rose_vec_add_kernel.cpp test_rule/rose_vec_add_kernel.ref
	cp -f rose_test.c test_rule/rose_test.ref
	cp -f rose_cnn_kernel.cpp test_rule/rose_cnn_kernel.ref
	cp -f rose___merlinkernel_cnn_kernel.c test_rule/rose___merlinkernel_cnn_kernel.ref 
diff:run 
	-diff --brief rose_matmul_kernel.cpp test_rule/rose_matmul_kernel.ref &>> err.log
	-diff --brief rose_matmul_ivdep_kernel.cpp test_rule/rose_matmul_ivdep_kernel.ref &>> err.log
	-diff --brief rose_vec_add_kernel.cpp test_rule/rose_vec_add_kernel.ref &>> err.log
	-diff --brief  rose_test.c test_rule/rose_test.ref &>> err.log
	-diff --brief rose_cnn_kernel.cpp test_rule/rose_cnn_kernel.ref &>> err.log
	-diff --brief rose___merlinkernel_cnn_kernel.c test_rule/rose___merlinkernel_cnn_kernel.ref &>> err.log

clean:
	rm -rf rose* *.o
