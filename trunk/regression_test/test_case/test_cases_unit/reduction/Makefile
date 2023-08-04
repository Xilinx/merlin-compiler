
all_test:   build unit_test

update_ref: update

unit_test: syntax diff

run:
	-rm -rf err.log
	-rm -rf rose* *.rpt
	-mars_opt regression_reduction/matmul_int/matmul_kernel_int.c regression_reduction/matmul_int/matmul.c \
    -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/matmul_float/matmul_kernel_float.c regression_reduction/matmul_float/matmul.c \
    -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/vec_add/vec_add_kernel.c regression_reduction/vec_add/vec_add.c \
    -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/vec_add_1/vec_add_kernel_1.c regression_reduction/vec_add_1/vec_add.c \
    -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/vec_add_2/vec_add_kernel_2.cpp regression_reduction/vec_add_2/vec_add.cpp \
    -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/idct/idct_kernel.c regression_reduction/idct/host.c \
    -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/idct_1/idct_kernel_1.c regression_reduction/idct_1/host.c \
    -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/logistic/logistic_regression_kernel.c \
    regression_reduction/logistic/logistic.c -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/logistic_L10_1/gradient_1.cpp regression_reduction/logistic_L10_1/host.cpp \
    -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/logistic_L10_2/gradient_2.cpp regression_reduction/logistic_L10_2/host.cpp \
     -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/basic_test/test.c -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/basic_test/test_err.c -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/basic_test/test_illegal.cpp -e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/run_namd/namd.cpp regression_reduction/run_namd/namd_kernel.cpp \
		-e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/run_flann/vec_add.cpp regression_reduction/run_flann/vec_add_kernel_flann.cpp \
		-e c -p reduction -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	#-mars_opt regression_reduction/basic_test/test_altera.c -e c -p reduction -a debug_mode=debug -a altera 2>tmp.log || (cat tmp.log >> err.log)

build:
	make -C ..

syntax: run
	-gcc -c -std=c99 -fsyntax-only -I regression_reduction/matmul_float rose_matmul_kernel_float.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only -I regression_reduction/matmul_int rose_matmul_kernel_int.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_vec_add_kernel.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_vec_add_kernel_1.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_vec_add_kernel_2.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only -I regression_reduction/idct/ rose_idct_kernel.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only -I regression_reduction/idct_1/ rose_idct_kernel_1.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only -I regression_reduction/logistic rose_logistic_regression_kernel.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only -I regression_reduction/logistic_L10_1 rose_gradient_1.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only -I regression_reduction/logistic_L10_2 rose_gradient_2.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_test.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_test_illegal.cpp >& tmp.log || (cat tmp.log >> err.log)


diff: run
	-diff --brief rose_matmul_kernel_float.c test_rule/matmul_float.out.ref &>> err.log
	-diff --brief rose_matmul_kernel_int.c test_rule/matmul_int.out.ref &>> err.log
	-diff --brief rose_vec_add_kernel.c test_rule/vec_add.out.ref &>> err.log
	-diff --brief rose_vec_add_kernel_1.c test_rule/vec_add_1.out.ref &>> err.log
	-diff --brief rose_vec_add_kernel_2.cpp test_rule/vec_add_2.out.ref &>> err.log
	-diff --brief rose_idct_kernel.c test_rule/idct.out.ref &>> err.log
	-diff --brief rose_idct_kernel_1.c test_rule/idct_1.out.ref &>> err.log
	-diff --brief rose_logistic_regression_kernel.c test_rule/logistic.out.ref &>> err.log
	-diff --brief rose_gradient_1.cpp test_rule/logistic_L10_1.out.ref &>> err.log
	-diff --brief rose_gradient_2.cpp test_rule/logistic_L10_2.out.ref &>> err.log
	-diff --brief rose_test.c test_rule/test.out.ref &>> err.log
	#-diff --brief rose_test_altera.c test_rule/test_altera.out.ref &>> err.log
	-diff --brief rose_test_illegal.cpp test_rule/test_illegal.out.ref &>> err.log
	-diff --brief rose_vec_add_kernel_flann.cpp test_rule/flann.out.ref &>> err.log
	-diff --brief rose_namd_kernel.cpp test_rule/namd.out.ref &>> err.log
	-diff --brief critical_message.rpt test_rule/critical_message.rpt.ref &&>> err.log

update:
	-cp rose_matmul_kernel_float.c test_rule/matmul_float.out.ref
	-cp rose_matmul_kernel_int.c test_rule/matmul_int.out.ref
	-cp rose_vec_add_kernel.c test_rule/vec_add.out.ref
	-cp rose_vec_add_kernel_1.c test_rule/vec_add_1.out.ref
	-cp rose_vec_add_kernel_2.cpp test_rule/vec_add_2.out.ref
	-cp rose_idct_kernel.c test_rule/idct.out.ref
	-cp rose_idct_kernel_1.c test_rule/idct_1.out.ref
	-cp rose_logistic_regression_kernel.c test_rule/logistic.out.ref
	-cp rose_gradient_1.cpp test_rule/logistic_L10_1.out.ref
	-cp rose_gradient_2.cpp test_rule/logistic_L10_2.out.ref
	-cp rose_test.c test_rule/test.out.ref
	-cp rose_test_illegal.cpp test_rule/test_illegal.out.ref
	#-cp rose_test_altera.c test_rule/test_altera.out.ref
	-cp rose_vec_add_kernel_flann.cpp test_rule/flann.out.ref
	-cp rose_namd_kernel.cpp test_rule/namd.out.ref
	-cp critical_message.rpt test_rule/critical_message.rpt.ref

