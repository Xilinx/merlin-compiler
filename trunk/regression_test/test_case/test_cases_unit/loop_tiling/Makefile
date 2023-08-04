


test:   build
#	mars_opt matmul/matmul_kernel_0.c -e c -p memory_burst 
#	make diff

#all_test:   build unit_test
gen_ref: update_ref
unit_test: diff 
run:
	-rm -rf err.log
	-rm -rf rose_*
	-rm -rf critical_*
	-mars_opt ./regression_test/test_1/vec_add.c ./regression_test/test_1/vec_add_kernel_test_1.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_2/vec_add.c ./regression_test/test_2/vec_add_kernel_test_2.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_3/vec_add.c ./regression_test/test_3/vec_add_kernel_test_3.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_4/vec_add.c ./regression_test/test_4/vec_add_kernel_test_4.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_5/vec_add.c ./regression_test/test_5/vec_add_kernel_test_5.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_6/vec_add.c ./regression_test/test_6/vec_add_kernel_test_6.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_7/vec_add.c ./regression_test/test_7/vec_add_kernel_test_7.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_8/vec_add.c ./regression_test/test_8/vec_add_kernel_test_8.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_9/vec_add.c ./regression_test/test_9/vec_add_kernel_test_9.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_10/vec_add.c ./regression_test/test_10/vec_add_kernel_test_10.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_11/vec_add.c ./regression_test/test_11/vec_add_kernel_test_11.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_12/vec_add.c ./regression_test/test_12/vec_add_kernel_test_12.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_13/vec_add.c ./regression_test/test_13/vec_add_kernel_test_13.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_14/vec_add.c ./regression_test/test_14/vec_add_kernel_test_14.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_15/vec_add.c ./regression_test/test_15/vec_add_kernel_test_15.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_16/vec_add.c ./regression_test/test_16/vec_add_kernel_test_16.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_16_1/vec_add.c ./regression_test/test_16_1/vec_add_kernel_test_16_1.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_16_2/vec_add.c ./regression_test/test_16_2/vec_add_kernel_test_16_2.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_17/vec_add.c ./regression_test/test_17/vec_add_kernel_test_17.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_18/vec_add.c ./regression_test/test_18/vec_add_kernel_test_18.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_19/vec_add.c ./regression_test/test_19/vec_add_kernel_test_19.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_20/vec_add.c ./regression_test/test_20/vec_add_kernel_test_20.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_21/vec_add.c ./regression_test/test_21/vec_add_kernel_test_21.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_22/vec_add.c ./regression_test/test_22/vec_add_kernel_test_22.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_23/vec_add.c ./regression_test/test_23/vec_add_kernel_test_23.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_24/vec_add.c ./regression_test/test_24/vec_add_kernel_test_24.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_25/vec_add.c ./regression_test/test_25/vec_add_kernel_test_25.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_26/vec_add.c ./regression_test/test_26/vec_add_kernel_test_26.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_27/vec_add.c ./regression_test/test_27/vec_add_kernel_test_27.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_28/vec_add.c ./regression_test/test_28/vec_add_kernel_test_28.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_label_indivisible/vec_add.c ./regression_test/test_label_indivisible/vec_add_kernel_test_label_indivisible.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_divisible/vec_add.c ./regression_test/test_divisible/vec_add_kernel_test_divisible.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_label_multi/vec_add.c ./regression_test/test_label_multi/vec_add_kernel_test_label_multi.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_label_multi2/vec_add.c ./regression_test/test_label_multi2/vec_add_kernel_test_label_multi2.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_label_goto/vec_add.c ./regression_test/test_label_goto/vec_add_kernel_test_label_goto.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_pragma_multi/vec_add.c ./regression_test/test_pragma_multi/vec_add_kernel_test_pragma_multi.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_pragma_odd1/vec_add.c ./regression_test/test_pragma_odd1/vec_add_kernel_test_pragma_odd1.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_pragma_odd2/vec_add.c ./regression_test/test_pragma_odd2/vec_add_kernel_test_pragma_odd2.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_pragma_special/vec_add_kernel_test_pragma_special.c -e c -p loop_tiling -a pragma_in_loop=yes
	-mars_opt ./regression_test/test_pragma_cache/vec_add.c ./regression_test/test_pragma_cache/vec_add_kernel_test_pragma_cache.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_pragma_cache_2/vec_add.c ./regression_test/test_pragma_cache_2/vec_add_kernel_test_pragma_cache_2.c -e c -p loop_tiling -a pragma_in_loop=yes
	-mars_opt ./regression_test/test_multi_1/vec_add.c ./regression_test/test_multi_1/vec_add_kernel_test_multi_1.c -e c -p loop_tiling
	-mars_opt ./regression_test/test_multi_2/vec_add.c ./regression_test/test_multi_2/vec_add_kernel_test_multi_2.c -e c -p loop_tiling -a pragma_in_loop=yes
	-mars_opt ./regression_test/test_multi_complete_with_factor/vec_add_kernel_multi_3.cpp -e c -p loop_tiling -a pragma_in_loop=no
	-mars_opt ./regression_test/test_intel/vec_add.c ./regression_test/test_intel/vec_add_kernel_test_intel.c -e c -p loop_tiling -a impl_tool=aocl
	-mars_opt ./regression_test/test_illegal/vec_add.c ./regression_test/test_illegal/vec_add_kernel_test_illegal.c -e c -p loop_tiling -a impl_tool=aocl
	-mars_opt ./regression_test/test_tripcount/vec_add.c ./regression_test/test_tripcount/vec_add_kernel_test_tripcount.c -e c -p loop_tiling -a impl_tool=aocl
#mars_opt ./regression_test/test_16/vec_add.c ./regression_test/test_16/vec_add_kernel_test_16.c -e c -p loop_tiling
#-mars_opt ./regression_test/test_2/vec_add.c ./regression_test/test_1/vec_add_kernel.c -e c -p loop_tiling 2>tmp.log || (cat tmp.log >> err.logs)




update_ref:run 
	cp -f ./rose_vec_add_kernel_test_1.c ./test_rule/rose_vec_add_kernel_test_1.c 
	cp -f ./rose_vec_add_kernel_test_2.c ./test_rule/rose_vec_add_kernel_test_2.c 
	cp -f ./rose_vec_add_kernel_test_3.c ./test_rule/rose_vec_add_kernel_test_3.c 
	cp -f ./rose_vec_add_kernel_test_4.c ./test_rule/rose_vec_add_kernel_test_4.c 
	cp -f ./rose_vec_add_kernel_test_5.c ./test_rule/rose_vec_add_kernel_test_5.c 
	cp -f ./rose_vec_add_kernel_test_6.c ./test_rule/rose_vec_add_kernel_test_6.c 
	cp -f ./rose_vec_add_kernel_test_7.c ./test_rule/rose_vec_add_kernel_test_7.c 
	cp -f ./rose_vec_add_kernel_test_8.c ./test_rule/rose_vec_add_kernel_test_8.c 
	cp -f ./rose_vec_add_kernel_test_9.c ./test_rule/rose_vec_add_kernel_test_9.c 
	cp -f ./rose_vec_add_kernel_test_10.c ./test_rule/rose_vec_add_kernel_test_10.c 
	cp -f ./rose_vec_add_kernel_test_11.c ./test_rule/rose_vec_add_kernel_test_11.c 
	cp -f ./rose_vec_add_kernel_test_12.c ./test_rule/rose_vec_add_kernel_test_12.c 
	cp -f ./rose_vec_add_kernel_test_13.c ./test_rule/rose_vec_add_kernel_test_13.c 
	cp -f ./rose_vec_add_kernel_test_14.c ./test_rule/rose_vec_add_kernel_test_14.c 
	cp -f ./rose_vec_add_kernel_test_15.c ./test_rule/rose_vec_add_kernel_test_15.c
	cp -f ./rose_vec_add_kernel_test_16.c ./test_rule/rose_vec_add_kernel_test_16.c
	cp -f ./rose_vec_add_kernel_test_16_1.c ./test_rule/rose_vec_add_kernel_test_16_1.c
	cp -f ./rose_vec_add_kernel_test_16_2.c ./test_rule/rose_vec_add_kernel_test_16_2.c
	cp -f ./rose_vec_add_kernel_test_17.c ./test_rule/rose_vec_add_kernel_test_17.c
	cp -f ./rose_vec_add_kernel_test_18.c ./test_rule/rose_vec_add_kernel_test_18.c
	cp -f ./rose_vec_add_kernel_test_19.c ./test_rule/rose_vec_add_kernel_test_19.c
	cp -f ./rose_vec_add_kernel_test_20.c ./test_rule/rose_vec_add_kernel_test_20.c
	cp -f ./rose_vec_add_kernel_test_21.c ./test_rule/rose_vec_add_kernel_test_21.c
	cp -f ./rose_vec_add_kernel_test_22.c ./test_rule/rose_vec_add_kernel_test_22.c
	cp -f ./rose_vec_add_kernel_test_23.c ./test_rule/rose_vec_add_kernel_test_23.c
	cp -f ./rose_vec_add_kernel_test_24.c ./test_rule/rose_vec_add_kernel_test_24.c
	cp -f ./rose_vec_add_kernel_test_25.c ./test_rule/rose_vec_add_kernel_test_25.c
	cp -f ./rose_vec_add_kernel_test_26.c ./test_rule/rose_vec_add_kernel_test_26.c
	cp -f ./rose_vec_add_kernel_test_27.c ./test_rule/rose_vec_add_kernel_test_27.c
	cp -f ./rose_vec_add_kernel_test_28.c ./test_rule/rose_vec_add_kernel_test_28.c
	cp -f ./rose_vec_add_kernel_test_label_indivisible.c ./test_rule/rose_vec_add_kernel_test_label_indivisible.c
	cp -f ./rose_vec_add_kernel_test_divisible.c ./test_rule/rose_vec_add_kernel_test_divisible.c
	cp -f ./rose_vec_add_kernel_test_label_multi.c ./test_rule/rose_vec_add_kernel_test_label_multi.c
	cp -f ./rose_vec_add_kernel_test_label_goto.c ./test_rule/rose_vec_add_kernel_test_label_goto.c
	cp -f ./rose_vec_add_kernel_test_pragma_multi.c ./test_rule/rose_vec_add_kernel_test_pragma_multi.c
	cp -f ./rose_vec_add_kernel_test_pragma_odd1.c ./test_rule/rose_vec_add_kernel_test_pragma_odd1.c
	cp -f ./rose_vec_add_kernel_test_pragma_odd2.c ./test_rule/rose_vec_add_kernel_test_pragma_odd2.c
	cp -f ./rose_vec_add_kernel_test_pragma_special.c ./test_rule/rose_vec_add_kernel_test_pragma_special.c.ref
	cp -f ./rose_vec_add_kernel_test_pragma_cache.c ./test_rule/rose_vec_add_kernel_test_pragma_cache.c
	cp -f ./rose_vec_add_kernel_test_pragma_cache_2.c ./test_rule/rose_vec_add_kernel_test_pragma_cache_2.c
	cp -f ./rose_vec_add_kernel_test_multi_1.c ./test_rule/rose_vec_add_kernel_test_multi_1.c
	cp -f ./rose_vec_add_kernel_test_multi_2.c ./test_rule/rose_vec_add_kernel_test_multi_2.c
	cp -f ./rose_vec_add_kernel_multi_3.cpp ./test_rule/rose_vec_add_kernel_multi_3.cpp
	cp -f ./rose_vec_add_kernel_test_intel.c ./test_rule/rose_vec_add_kernel_test_intel.c.ref
	cp -f ./rose_vec_add_kernel_test_illegal.c ./test_rule/rose_vec_add_kernel_test_illegal.c.ref
	cp -f ./rose_vec_add_kernel_test_tripcount.c ./test_rule/rose_vec_add_kernel_test_tripcount.c.ref
	cp -f ./critical_message.rpt ./test_rule/critical_message.rpt
#	cp -f ./rose_vec_add_kernel_test_label_multi2.c ./test_rule/rose_vec_add_kernel_test_label_multi2.c
	

diff:run 
	-diff --brief rose_vec_add_kernel_test_1.c ./test_rule/rose_vec_add_kernel_test_1.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_2.c ./test_rule/rose_vec_add_kernel_test_2.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_3.c ./test_rule/rose_vec_add_kernel_test_3.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_4.c ./test_rule/rose_vec_add_kernel_test_4.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_5.c ./test_rule/rose_vec_add_kernel_test_5.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_6.c ./test_rule/rose_vec_add_kernel_test_6.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_7.c ./test_rule/rose_vec_add_kernel_test_7.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_8.c ./test_rule/rose_vec_add_kernel_test_8.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_9.c ./test_rule/rose_vec_add_kernel_test_9.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_10.c ./test_rule/rose_vec_add_kernel_test_10.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_11.c ./test_rule/rose_vec_add_kernel_test_11.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_12.c ./test_rule/rose_vec_add_kernel_test_12.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_13.c ./test_rule/rose_vec_add_kernel_test_13.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_14.c ./test_rule/rose_vec_add_kernel_test_14.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_15.c ./test_rule/rose_vec_add_kernel_test_15.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_16.c ./test_rule/rose_vec_add_kernel_test_16.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_16_1.c ./test_rule/rose_vec_add_kernel_test_16_1.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_16_2.c ./test_rule/rose_vec_add_kernel_test_16_2.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_17.c ./test_rule/rose_vec_add_kernel_test_17.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_18.c ./test_rule/rose_vec_add_kernel_test_18.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_19.c ./test_rule/rose_vec_add_kernel_test_19.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_20.c ./test_rule/rose_vec_add_kernel_test_20.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_21.c ./test_rule/rose_vec_add_kernel_test_21.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_22.c ./test_rule/rose_vec_add_kernel_test_22.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_23.c ./test_rule/rose_vec_add_kernel_test_23.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_24.c ./test_rule/rose_vec_add_kernel_test_24.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_25.c ./test_rule/rose_vec_add_kernel_test_25.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_26.c ./test_rule/rose_vec_add_kernel_test_26.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_27.c ./test_rule/rose_vec_add_kernel_test_27.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_28.c ./test_rule/rose_vec_add_kernel_test_28.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_label_indivisible.c ./test_rule/rose_vec_add_kernel_test_label_indivisible.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_divisible.c ./test_rule/rose_vec_add_kernel_test_divisible.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_label_multi.c ./test_rule/rose_vec_add_kernel_test_label_multi.c &>> err.log
#	-diff --brief rose_vec_add_kernel_test_label_multi2.c ./test_rule/rose_vec_add_kernel_test_label_multi2.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_label_goto.c ./test_rule/rose_vec_add_kernel_test_label_goto.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_pragma_multi.c ./test_rule/rose_vec_add_kernel_test_pragma_multi.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_pragma_odd1.c ./test_rule/rose_vec_add_kernel_test_pragma_odd1.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_pragma_odd2.c ./test_rule/rose_vec_add_kernel_test_pragma_odd2.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_pragma_special.c ./test_rule/rose_vec_add_kernel_test_pragma_special.c.ref &>> err.log
	-diff --brief rose_vec_add_kernel_test_pragma_cache.c ./test_rule/rose_vec_add_kernel_test_pragma_cache.c  &>> err.log
	-diff --brief rose_vec_add_kernel_test_pragma_cache_2.c ./test_rule/rose_vec_add_kernel_test_pragma_cache_2.c &>> err.log
	-diff --brief rose_vec_add_kernel_test_multi_1.c ./test_rule/rose_vec_add_kernel_test_multi_1.c &>> err.log
	-diff --brief rose_vec_add_kernel_multi_3.cpp ./test_rule/rose_vec_add_kernel_multi_3.cpp &>> err.log
	-diff --brief rose_vec_add_kernel_test_intel.c ./test_rule/rose_vec_add_kernel_test_intel.c.ref &>> err.log
	-diff --brief rose_vec_add_kernel_test_illegal.c ./test_rule/rose_vec_add_kernel_test_illegal.c.ref &>> err.log
	-diff --brief rose_vec_add_kernel_test_tripcount.c ./test_rule/rose_vec_add_kernel_test_tripcount.c.ref &>> err.log
	-diff --brief critical_message.rpt ./test_rule/critical_message.rpt &>> err.log


debug:
	gdb -args mars_opt regression_man/lab1/vec_add.c -e c -p memory_burst
clean:
	rm -rf rose* *.o
