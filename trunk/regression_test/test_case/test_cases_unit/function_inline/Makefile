cases=test_single_call.c \
	  test_multiple_call.c \
	  test_two_level.c \
	  test_diamond_call.c \
	  test_local_var.c  \
	  test_const_global_var.c \
	  test_redundant_global.c \
	  test_multi_array.cpp \
	  test_overload_func.cpp \
	  test_same_var.cpp 

#test_clean_func.c
unsupported_cases= 	  test_global_var.c \
					  test_middle_return.c \
					  test_void_return.c \
					  test_goto_stmt.c  \
					  test_kernel_host.c

pragma_cases= 		test_pragma_1.c \
			test_pragma_2.c \
		        test_pragma_3.c \
		        test_pragma_4.c \
		        test_pragma_5.c \
		        test_pragma_6.c

all_test:   build unit_test
gen_ref: update_ref
unit_test: diff 
run:
	-rm -rf err.log
	-rm -rf critical_message.rpt
	touch err.log
	-for f in $(cases); \
	  do \
	  mars_opt $$f -e c -p function_inline 2>tmp.log || (cat tmp.log >> err.log); \
	  done
	  -mars_opt test_top.c test_top_sub.c -e c -p function_inline
	-for f in $(unsupported_cases); \
	  do \
	  mars_opt $$f -e c -p function_inline ; \
	  done
	  -mars_opt test_global_var.c -e c -p function_inline -a impl_tool=aocl -a effort=low
	-for f in $(pragma_cases); \
	  do \
	  mars_opt $$f -e c -p function_inline_by_pragma_HLS 2>tmp.log || (cat tmp.log >> err.log); \
	  done


update_ref:run 
	-for f in $(cases); \
	  do \
	  	base_f=`basename $$f`; \
	  cp rose_$${base_f} test_rule/rose_$${base_f}; \
	  done
	-for f in $(pragma_cases); \
	  do \
	  	base_f=`basename $$f`; \
	  cp rose_$${base_f} test_rule/rose_$${base_f}; \
	  done
	-cp  critical_message.rpt test_rule/critical_message.ref


diff:run 
	-for f in $(cases); \
	  do \
	  	base_f=`basename $$f`; \
		gcc -fsyntax-only -std=c99 rose_$${base_f} 2>tmp.log || cat tmp.log >> err.log; \
	  diff -du rose_$${base_f} test_rule/rose_$${base_f} &>> err.log; \
	  done
	-for f in $(pragma_cases); \
	  do \
	  	base_f=`basename $$f`; \
		gcc -fsyntax-only -std=c99 rose_$${base_f} 2>tmp.log || cat tmp.log >> err.log; \
	  diff -du rose_$${base_f} test_rule/rose_$${base_f} &>> err.log; \
	  done
	-diff -du  critical_message.rpt test_rule/critical_message.ref &>>err.log

clean:
	rm -rf rose* *.o
