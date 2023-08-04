exe_cases = test_scalar_sim

build:
	make -C ..

all_test:   build unit_test

unit_test: run
	-make syntax
	make compare

all_test_naive: all_run_naive compare_naive
	@rm -rf rose.log rose_succeed

run: unit_case
	-mars_opt regression_coarse/segmentation/seg_0_kernel.c regression_coarse/segmentation/segmentation.c -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/lab2/vec_add_kernel.c regression_coarse/lab2/vec_add.c -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/idct/idct_kernel.c regression_coarse/idct/host.c -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/dwt/dwt3d_kernel.c regression_coarse/dwt/dwt3d_main_a.c  -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/logistic/logistic_regression_kernel.c -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/matmul/matmul_kernel.c regression_coarse/matmul/matmul.c -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/blackschole/bs_kernel.c regression_coarse/blackschole/blackscholes.c  -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/denoise/denoise.c regression_coarse/denoise/main.c -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt  regression_coarse/run_logisticL10/gradient.cpp  regression_coarse/run_logisticL10/host.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt  regression_coarse/run_normal/normal_kernel.cpp  regression_coarse/run_normal/testNormalC.cpp \
		-e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/twoNN/twonn_kernel.c regression_coarse/twoNN/twonn.c -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/run_logistic64/logistic.cpp regression_coarse/run_logistic64/logistic64.cpp \
		-e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)

unit_case: diff_exe
	-rm -rf critical_message.rpt
	-mars_opt regression_coarse/basic_test/test.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel -a naive_hls | tee rose_test.c.log
	-mv rose_test.cpp rose_test_naive.cpp
	-mars_opt regression_coarse/basic_test/test.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test2.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_iterator.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_label.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_dowhile.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_while.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_imperfect.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_subfun1.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_subfun3.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_if2.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_break.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_localdecl.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_scalar.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_postprocess.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_apint.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel -I ${MERLIN_COMPILER_HOME}/source-opt/include/xf_opencv 2>tmp.log || (cat tmp.log >> err.log) 
	-mars_opt regression_coarse/basic_test/test_range.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_coarse/basic_test/test_apint_initializer.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)

all_run_naive:
	mars_opt regression_coarse/basic_test/test.c -e c -p coarse_pipeline -a impl_tool=sdaccel -a naive_hls | tee rose_test.c.log
	gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test.c
	mars_opt regression_coarse/basic_test/test2.c -e c -p coarse_pipeline -a impl_tool=sdaccel  -a naive_hls | tee rose_test2.c.log
	gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test2.c
	mars_opt regression_coarse/segmentation/seg_0_kernel.c -e c -p coarse_pipeline -a impl_tool=sdaccel -a naive_hls  | tee rose_seg_0.c.log
	gcc -c -fsyntax-only  -I regression_coarse/segmentation rose_seg_0_kernel.c
	mars_opt regression_coarse/lab2/vec_add_kernel.c -e c -p coarse_pipeline -a impl_tool=sdaccel -a naive_hls  | tee rose_vec_add.c.log
	gcc -c -fsyntax-only  -I regression_coarse/lab2 rose_vec_add_kernel.c
	mars_opt regression_coarse/idct/idct_kernel.c -e c -p coarse_pipeline -a impl_tool=sdaccel -a naive_hls  | tee rose_idct.c.log
	gcc -c -fsyntax-only  -I regression_coarse/idct  rose_idct_kernel.c
	mars_opt regression_coarse/dwt/dwt3d_kernel.c -e c -p coarse_pipeline -a impl_tool=sdaccel -a naive_hls  | tee rose_dwt3d.c.log
	gcc -c -fsyntax-only  -I regression_coarse/dwt rose_dwt3d_kernel.c
	mars_opt regression_coarse/logistic/logistic_regression_kernel.c regression_coarse/logistic/logistic.c -e c -p coarse_pipeline -a impl_tool=sdaccel -a naive_hls  | tee rose_logistic.c.log
	gcc -c -fsyntax-only  -I regression_coarse/logistic rose_logistic_regression_kernel.c
	mars_opt regression_coarse/matmul/matmul_kernel.c -e c -p coarse_pipeline -a impl_tool=sdaccel -a naive_hls  | tee rose_matmul.c.log
	gcc -c -fsyntax-only  -I regression_coarse/matmul  rose_matmul_kernel.c
	mars_opt regression_coarse/blackschole/bs_kernel.c -e c -p coarse_pipeline -a impl_tool=sdaccel -a naive_hls  | tee rose_bs.c.log
	gcc -c -fsyntax-only  -I regression_coarse/blackschole  rose_bs_kernel.c
	mars_opt regression_coarse/denoise/denoise.c -e c -p coarse_pipeline -a impl_tool=sdaccel -a naive_hls  | tee rose_denoise.c.log
	gcc -c -fsyntax-only  -I regression_coarse/denoise rose_denoise.c
	mars_opt regression_coarse/twoNN/twonn_kernel.c regression_coarse/twoNN/twonn.c -e c -p coarse_pipeline -a impl_tool=sdaccel  -a naive_hls | tee rose_twonn.c.log
	gcc -c -fsyntax-only  -I regression_coarse/twoNN rose_twonn_kernel.c


clean_ref:
	rm -rf rose_seg_0_kernel.c test_rule/seg_0.out.ref
	rm -rf rose_vec_add_kernel.c test_rule/vec_add.out.ref
	rm -rf rose_idct_kernel.c test_rule/idct.out.ref
	rm -rf rose_logistic_regression_kernel.c test_rule/logistic.out.ref
	rm -rf rose_matmul_kernel.c test_rule/matmul.out.ref
	rm -rf rose_dwt3d_kernel.c test_rule/dwt3d.out.ref
	rm -rf rose_bs_kernel.c test_rule/bs.out.ref
	rm -rf rose_test_naive.c test_rule/test.out.naive.ref
	rm -rf rose_test.cpp test_rule/test.out.ref
	rm -rf rose_test2.cpp test_rule/test2.out.ref
	rm -rf rose_test_iterator.cpp test_rule/test_iterator.out.ref
	rm -rf rose_test_label.cpp test_rule/test_label.out.ref
	rm -rf rose_test_dowhile.cpp test_rule/test_dowhile.out.ref
	rm -rf rose_test_while.cpp test_rule/test_while.out.ref
	rm -rf rose_test_imperfect.cpp test_rule/test_imperfect.out.ref
	rm -rf rose_test_subfun1.cpp test_rule/test_subfun1.out.ref
	rm -rf rose_test_subfun3.cpp test_rule/test_subfun3.out.ref
	rm -rf rose_test_if2.cpp test_rule/test_if2.out.ref
	rm -rf rose_test_break.cpp test_rule/test_break.out.ref
	rm -rf rose_test_localdecl.cpp test_rule/test_localdecl.out.ref
	rm -rf rose_test_scalar.cpp test_rule/test_scalar.out.ref
	rm -rf rose_test_postprocess.cpp test_rule/test_postprocess.out.ref
	rm -rf rose_test_apint.cpp test_rule/test_apint.out.ref
	rm -rf rose_twonn_kernel.c test_rule/twonn.out.ref
	rm -rf rose_denoise.c test_rule/denoise.out.ref

gen_ref: clean_ref run
	mv rose_seg_0_kernel.c  test_rule/seg_0.out.ref
	mv rose_vec_add_kernel.c  test_rule/vec_add.out.ref
	mv rose_idct_kernel.c  test_rule/idct.out.ref
	mv rose_logistic_regression_kernel.c  test_rule/logistic.out.ref
	mv rose_matmul_kernel.c  test_rule/matmul.out.ref
	mv rose_dwt3d_kernel.c  test_rule/dwt3d.out.ref
	mv rose_bs_kernel.c  test_rule/bs.out.ref
	mv rose_test_naive.cpp  test_rule/test.out.naive.ref
	mv rose_test.cpp  test_rule/test.out.ref
	mv rose_test2.cpp  test_rule/test2.out.ref
	mv rose_test_iterator.cpp  test_rule/test_iterator.out.ref
	mv rose_test_label.cpp  test_rule/test_label.out.ref
	mv rose_test_dowhile.cpp  test_rule/test_dowhile.out.ref
	mv rose_test_while.cpp  test_rule/test_while.out.ref
	mv rose_test_imperfect.cpp  test_rule/test_imperfect.out.ref
	mv rose_test_subfun1.cpp  test_rule/test_subfun1.out.ref
	mv rose_test_subfun3.cpp  test_rule/test_subfun3.out.ref
	mv rose_test_if2.cpp  test_rule/test_if2.out.ref
	mv rose_test_break.cpp  test_rule/test_break.out.ref
	mv rose_test_localdecl.cpp  test_rule/test_localdecl.out.ref
	mv rose_test_scalar.cpp  test_rule/test_scalar.out.ref
	mv rose_test_postprocess.cpp  test_rule/test_postprocess.out.ref
	mv rose_test_apint.cpp  test_rule/test_apint.out.ref
	mv rose_test_range.cpp  test_rule/test_range.out.ref
	mv rose_test_apint_initializer.cpp  test_rule/test_apint_initializer.out.ref
	mv rose_twonn_kernel.c  test_rule/twonn.out.ref
	mv rose_denoise.c test_rule/denoise.out.ref
	mv rose_gradient.cpp test_rule/gradient.out.ref
	mv rose_normal_kernel.cpp test_rule/normal.out.ref
	mv rose_logistic64.cpp test_rule/logistic64.out.ref
	mv critical_message.rpt test_rule/critical_message.rpt.ref

compare: compare_unit_case
	-diff --brief rose_seg_0_kernel.c test_rule/seg_0.out.ref &>> err.log
	-diff --brief rose_vec_add_kernel.c test_rule/vec_add.out.ref &>> err.log
	-diff --brief rose_idct_kernel.c test_rule/idct.out.ref &>> err.log
	-diff --brief rose_logistic_regression_kernel.c test_rule/logistic.out.ref &>> err.log
	-diff --brief rose_matmul_kernel.c test_rule/matmul.out.ref &>> err.log
	-diff --brief rose_dwt3d_kernel.c test_rule/dwt3d.out.ref &>> err.log
	-diff --brief rose_bs_kernel.c test_rule/bs.out.ref &>> err.log
	-diff --brief rose_denoise.c test_rule/denoise.out.ref &>> err.log
	-diff --brief rose_twonn_kernel.c test_rule/twonn.out.ref &>> err.log
	-diff --brief rose_gradient.cpp test_rule/gradient.out.ref &>> err.log
		-diff --brief rose_normal_kernel.cpp test_rule/normal.out.ref &>> err.log
	-diff --brief rose_logistic64.cpp test_rule/logistic64.out.ref &>> err.log
	-diff --brief critical_message.rpt test_rule/critical_message.rpt.ref &>> err.log

compare_unit_case:
	-diff --brief rose_test_naive.cpp  test_rule/test.out.naive.ref &>> err.log
	-diff --brief rose_test.cpp  test_rule/test.out.ref &>> err.log
	-diff --brief rose_test2.cpp  test_rule/test2.out.ref &>> err.log
	-diff --brief rose_test_iterator.cpp  test_rule/test_iterator.out.ref &>> err.log
	-diff --brief rose_test_label.cpp  test_rule/test_label.out.ref &>> err.log
	-diff --brief rose_test_dowhile.cpp  test_rule/test_dowhile.out.ref &>> err.log
	-diff --brief rose_test_while.cpp  test_rule/test_while.out.ref &>> err.log
	-diff --brief rose_test_imperfect.cpp  test_rule/test_imperfect.out.ref &>> err.log
	-diff --brief rose_test_subfun1.cpp  test_rule/test_subfun1.out.ref &>> err.log
	-diff --brief rose_test_subfun3.cpp  test_rule/test_subfun3.out.ref &>> err.log
	-diff --brief rose_test_if2.cpp  test_rule/test_if2.out.ref &>> err.log
	-diff --brief rose_test_break.cpp  test_rule/test_break.out.ref &>> err.log
	-diff --brief rose_test_localdecl.cpp  test_rule/test_localdecl.out.ref &>> err.log
	-diff --brief rose_test_scalar.cpp  test_rule/test_scalar.out.ref &>> err.log
	-diff --brief rose_test_postprocess.cpp  test_rule/test_postprocess.out.ref &>> err.log
	-diff --brief rose_test_apint.cpp  test_rule/test_apint.out.ref &>> err.log
	-diff --brief rose_test_range.cpp  test_rule/test_range.out.ref &>> err.log
	-diff --brief rose_test_apint_initializer.cpp  test_rule/test_apint_initializer.out.ref &>> err.log

compare_naive:
	-diff --brief rose_seg_0_kernel.c test_rule/seg_0.out.naive.ref
	-diff --brief rose_vec_add_kernel.c test_rule/vec_add.out.naive.ref
	-diff --brief rose_idct_kernel.c test_rule/idct.out.naive.ref
	-diff --brief rose_logistic_regression_kernel.c test_rule/logistic.out.naive.ref
	-diff --brief rose_matmul_kernel.c test_rule/matmul.out.naive.ref
	-diff --brief rose_dwt3d_kernel.c test_rule/dwt3d.out.naive.ref
	-diff --brief rose_bs_kernel.c test_rule/bs.out.naive.ref
	-diff --brief rose_test.c  test_rule/test.out.naive.ref
	-diff --brief rose_test2.c  test_rule/test2.out.naive.ref
	-diff --brief rose_denoise.c test_rule/denoise.out.naive.ref
	#-diff --brief rose_twonn_kernel.c test_rule/twonn.out.naive.ref

syntax:
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_naive.cpp  >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test.cpp  >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test2.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_iterator.cpp  >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_label.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_dowhile.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_while.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_imperfect.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_subfun1.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_subfun3.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_if2.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_break.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_localdecl.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_scalar.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  rose_test_postprocess.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  -I ${MERLIN_COMPILER_HOME}/source-opt/include/apint_include  -I ${MERLIN_COMPILER_HOME}/source-opt/include/xf_opencv rose_test_apint.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/basic_test  -I ${MERLIN_COMPILER_HOME}/source-opt/include/apint_include rose_test_apint_initializer.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/segmentation rose_seg_0_kernel.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/lab2 rose_vec_add_kernel.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/idct  rose_idct_kernel.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/dwt rose_dwt3d_kernel.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/logistic rose_logistic_regression_kernel.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/matmul  rose_matmul_kernel.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/denoise rose_denoise.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -fsyntax-only  -I regression_coarse/blackschole  rose_bs_kernel.c >& tmp.log || (cat tmp.log >> err.log)

clean:
	rm -rf *.o *.log *.c *.cpp
run_exe:
	-rm -rf *.res *.out *.log *.rpt
	-rm -rf rose*
	-for f in ${exe_cases}; \
	  do \
			mars_opt test_sim/$$f.cpp -e c -p coarse_pipeline -a impl_tool=sdaccel >& tmp1.log || (cat tmp1.log >> err.log); \
			g++ rose_$$f.cpp >& tmp1.log || (cat tmp1.log >> err.log); \
			./a.out  >& tmp1.log || (cat tmp1.log >> err.log); \
            rm a.out; \
	  done
diff_exe:run_exe
	 -for f in ${exe_cases}; \
	  do \
			diff --brief rose_$$f.cpp test_rule/$$f.out.ref &>> err.log; \
	  done
update_exe_ref:run_exe
	 -for f in ${exe_cases}; \
	  do \
	  cp rose_$$f.cpp test_rule/$$f.out.ref; \
	  done
	-cd ../
