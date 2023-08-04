cases=basic_test.cpp            fine_grain_test.c  pragma9.c             test_compound_exp.c    \
bracket_test.c            flatten_test.c     test_addressof_op.c   test_compound_exp.cpp  test_precheck.cpp \
bug-1690.cpp              pragma10.cpp       test_assert.c         test_cond_exp.cpp      test_sizeof_op.c \
complex_canonical_loop.c  pragma8.cpp        test_canonical_1.cpp  test_vendor_pragma.cpp test_err.cpp
unit_test: diff $(patsubst %,diff-%,$(cases))
	cat $(patsubst %,run_%/err.log,$(cases)) >> err.log
run: 
	-mars_opt regression_preprocess/pragma/pragma.cpp  regression_preprocess/pragma/test.cpp -e c -p preprocess -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_preprocess/pragma/pragma2.cpp  regression_preprocess/pragma/test2.cpp -e c -p preprocess -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_preprocess/pragma/pragma3.cpp  regression_preprocess/pragma/test3.cpp -e c -p preprocess -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_preprocess/pragma/pragma4.cpp  -e c -p preprocess -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_preprocess/pragma/pragma5.cpp  -e c -p preprocess -a impl_tool=sdaccel -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_preprocess/pragma/pragma6.cpp  -e c -p preprocess -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_preprocess/pragma/pragma7.cpp  -e c -p preprocess -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_preprocess/pragma/pragma_invalid.cpp  -e c -p preprocess -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_preprocess/pragma/pragma_include.cpp -e c -p preprocess -I regression_preprocess/pragma/ 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_preprocess/pragma/pragma_with_non_numeric_option.cpp  -e c -p preprocess -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_preprocess/pragma/pragma_with_enumerator.cpp  -e c -p preprocess -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_pragma.rpt
	-mars_opt -I${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin regression_preprocess/math_list/math_list_intel.c -e c -p preprocess -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt -I${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin regression_preprocess/math_list/math_list_xilinx.c -e c -p preprocess -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-cd regression_preprocess/bugs/665 && mars_opt sha.c sha_driver.c  -e c -p preprocess -a naive_hls  \
	  -a impl_tool=aocl -a naive_hls -a debug_mode=debug || (cat tmp.log >> err.log)
	-cd regression_preprocess/bugs/819 && mars_opt  test_range_1.c  -e c -p preprocess  -a impl_tool=aocl \
	  -a effort=medium  -a impl_tool=aocl  -a debug_mode=debug  || (cat tmp.log >> err.log)
	-cd regression_preprocess/bugs/938 && mars_opt  host.cpp svm_kernel.cpp  -e c -p preprocess \
	  -a impl_tool=aocl -a effort=medium  -a impl_tool=aocl  -a debug_mode=debug || echo fails >> err.log
	-mars_opt regression_preprocess/xilinx_stl/test_xilinx_stl.cpp -e c -p preprocess -a impl_tool=sdaccel 2>tmp.log || cat tmp.log >> err.log
	-mars_opt regression_preprocess/xilinx_stl/test_xilinx_stl.cpp -e c -p preprocess -a impl_tool=aocl 
	-touch critical_message.rpt
	-mv critical_message.rpt critical_message_other.rpt
	-mars_opt regression_preprocess/local_static/test_local_static.cpp -e c -p preprocess -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_preprocess/msg/test20_1/src/linear_check.c regression_preprocess/msg/test20_1/src/test20_1.c -e c -p preprocess -a impl_tool=aocl -a effort=medium
	-mv critical_message.rpt critical_message_20_1.rpt
	-mars_opt regression_preprocess/msg/test20_2/src/renaming_check.c regression_preprocess/msg/test20_2/src/test20_2.c -e c -p preprocess -a impl_tool=aocl -a effort=medium
	-mv critical_message.rpt critical_message_20_2.rpt
	-mars_opt regression_preprocess/msg/test20_3/src/while_check.c regression_preprocess/msg/test20_3/src/test20_3.c -e c -p preprocess -a impl_tool=aocl -a effort=medium
	-mv critical_message.rpt critical_message_20_3.rpt
	-mars_opt regression_preprocess/msg/test20_4/src/break_check.c regression_preprocess/msg/test20_4/src/test20_4.c -e c -p preprocess -a impl_tool=aocl -a effort=medium
	-mv critical_message.rpt critical_message_20_4.rpt
	-mars_opt regression_preprocess/msg/test20_5/src/if_check.c regression_preprocess/msg/test20_5/src/test20_5.c -e c -p preprocess -a impl_tool=aocl -a effort=medium
	-mv critical_message.rpt critical_message_20_5.rpt
	-mars_opt regression_preprocess/counter_example/test.cpp -e c -p preprocess -a impl_tool=aocl -a effort=medium -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_counter_example.rpt
	-mars_opt regression_preprocess/unused_interface_pragma/unused_interface_pragma.cpp -e c -p preprocess -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_unused_pragma.rpt
	-touch critical_message.rpt
	-mars_opt regression_preprocess/unsupported_type/test_unsupported_type.c -e c -p preprocess -a pure_kernel=on -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_unsupported_type_xilinx.rpt
#	-mars_opt regression_preprocess/large_const/__merlinkernel_svm_top.cpp -e c -p preprocess -a impl_tool=aocl -a effort=standard -a debug_mode=debug -a cflags="-D INTEL"
#	-mv critical_message.rpt critical_message_large_const.rpt
	-mars_opt regression_preprocess/check_arg_number_intel/test1.cpp -e c -p preprocess -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_arg_number_intel.rpt
	touch critical_message.rpt
	-mars_opt regression_preprocess/local_array_check/check_local_array_size.cpp -e c -p preprocess -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_local_array_size.rpt

gen_ref: update

update:run $(patsubst %,update-%,$(cases)) 
	-cp rose_test_local_static.cpp test_rule/rose_test_local_static.cpp 
	-cp rose_pragma2.cpp test_rule/pragma2.out.ref 
	-cp rose_pragma3.cpp test_rule/pragma3.out.ref 
	-cp rose_pragma6.cpp test_rule/pragma6.out.ref 
	-cp rose_pragma_with_non_numeric_option.cpp test_rule/pragma_with_non_numeric_option.out.ref 
	-cp rose_pragma_with_enumerator.cpp test_rule/pragma_with_enumerator.out.ref 
	-cp regression_preprocess/bugs/665/rose_sha.c test_rule/bugs/sha.out.ref 
	-cp regression_preprocess/bugs/819/critical_message.rpt test_rule/bugs/critical_message_819.ref  
	-cp regression_preprocess/bugs/938/critical_message.rpt test_rule/bugs/critical_message_938.ref 
	-cp critical_message_other.rpt test_rule/critical_message.out.ref 
	-cp critical_message_20_1.rpt test_rule/msg/critical_message_20_1.ref
	-cp critical_message_20_2.rpt test_rule/msg/critical_message_20_2.ref
	-cp critical_message_20_3.rpt test_rule/msg/critical_message_20_3.ref
	-cp critical_message_20_4.rpt test_rule/msg/critical_message_20_4.ref
	-cp critical_message_20_5.rpt test_rule/msg/critical_message_20_5.ref 
	-cp critical_message_counter_example.rpt test_rule/msg_critical_message_counter_example.ref
	-cp rose_test_xilinx_stl.cpp test_rule/test_xilinx_stl.out.ref
	-cp critical_message_unused_pragma.rpt test_rule/msg_critical_message_unused_pragma.ref
	-cp critical_message_pragma.rpt test_rule/msg_critical_message_pragma.ref
	-cp critical_message_unsupported_type_xilinx.rpt test_rule/msg_critical_message_unsupported_type_xilinx.ref 
#	-cp critical_message_large_const.rpt test_rule/critical_message_large_const.ref
	-cp critical_message_check_arg_number_intel.rpt test_rule/critical_message_check_arg_number_intel.rpt
	-cp critical_message_check_local_array_size.rpt test_rule/critical_message_check_local_array_size.rpt

all_test_naive:

diff: run
	-diff --brief rose_test_local_static.cpp  test_rule/rose_test_local_static.cpp &>> err.log
	-diff --brief rose_test_xilinx_stl.cpp test_rule/test_xilinx_stl.out.ref &>>err.log
	-diff --brief rose_pragma2.cpp test_rule/pragma2.out.ref &>> err.log
	-diff --brief rose_pragma3.cpp test_rule/pragma3.out.ref &>> err.log
	-diff --brief rose_pragma_with_non_numeric_option.cpp test_rule/pragma_with_non_numeric_option.out.ref &>> err.log
	-diff --brief rose_pragma_with_enumerator.cpp test_rule/pragma_with_enumerator.out.ref &>> err.log
	-diff --brief regression_preprocess/bugs/665/rose_sha.c test_rule/bugs/sha.out.ref &>> err.log
	-diff --brief regression_preprocess/bugs/819/critical_message.rpt test_rule/bugs/critical_message_819.ref &>>err.log 
	-diff --brief regression_preprocess/bugs/938/critical_message.rpt test_rule/bugs/critical_message_938.ref &>>err.log
	-diff --brief critical_message_other.rpt test_rule/critical_message.out.ref &>> err.log
	-diff --brief critical_message_20_1.rpt test_rule/msg/critical_message_20_1.ref &>> err.log
	-diff --brief critical_message_20_2.rpt test_rule/msg/critical_message_20_2.ref &>> err.log
	-diff --brief critical_message_20_3.rpt test_rule/msg/critical_message_20_3.ref &>> err.log
	-diff --brief critical_message_20_4.rpt test_rule/msg/critical_message_20_4.ref &>> err.log
	-diff --brief critical_message_20_5.rpt test_rule/msg/critical_message_20_5.ref &>> err.log
	-diff --brief critical_message_counter_example.rpt test_rule/msg_critical_message_counter_example.ref &>> err.log
	-diff --brief critical_message_unused_pragma.rpt test_rule/msg_critical_message_unused_pragma.ref &>> err.log
	-diff --brief critical_message_pragma.rpt test_rule/msg_critical_message_pragma.ref &>> err.log
	-diff --brief  critical_message_unsupported_type_xilinx.rpt test_rule/msg_critical_message_unsupported_type_xilinx.ref &>>err.log
#	-diff --brief critical_message_large_const.rpt test_rule/critical_message_large_const.ref &> err.log
#   -diff --brief rose_pragma.cpp test_rule/pragma.out.ref &>> err.log
	-diff --brief critical_message_check_arg_number_intel.rpt test_rule/critical_message_check_arg_number_intel.rpt &>>err.log
	-diff --brief critical_message_check_local_array_size.rpt test_rule/critical_message_check_local_array_size.rpt &>>err.log

diff-%: run-%
	$(eval @_case_ := $(patsubst diff-%,%,$@))
	-diff --brief run_$(@_case_)/rose_$(@_case_) test_rule/rose_$(@_case_) &>> run_$(@_case_)/err.log
update-%: run-%
	$(eval @_case_ := $(patsubst update-%,%,$@))
	cp run_$(@_case_)/rose_$(@_case_) test_rule/rose_$(@_case_)
run-%: %
	-rm -rf run_$^; mkdir run_$^; cd run_$^; touch err.log; \
	  mars_opt ../$^ -e c -p preprocess 2>tmp.log || cat tmp.log > err.log; \
	  gcc -std=c99 -fsyntax-only -I ${MERLIN_COMPILER_HOME}/source-opt/include/apint_include  \
	  rose_$^ 2>tmp.log || cat tmp.log >> err.log

clean:
	-rm -rf regression_preprocess/bugs/*/*.log
	-rm -rf regression_preprocess/bugs/*/*.o
	-rm -rf regression_preprocess/bugs/*/rose*
	-rm -rf regression_preprocess/bugs/*/*.rpt
	-rm -rf regression_preprocess/bugs/*/*.json
	-rm -rf regression_preprocess/bugs/*/*.ti
	-rm -rf regression_preprocess/bugs/*/__merlin_kernel_list.h
	-rm -rf regression_preprocess/bugs/*/.finish_read_files
	-rm -rf *.log *.o rose* *.json *.h *.ti
	-rm critical_message* conflict_type_def
