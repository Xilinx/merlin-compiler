correct_dir=test_basic test_sub_function_call test_struct \
			test_class test_typedef \
			test_struct_multi test_class_multi test_typedef_multi \
			test_recursive_typedef \
			test_typedef_builtin test_struct_ref \
			test_enum_cast \
			test_enum_val \
			test_system_type \
			test_header_file \
			test_bank test_bank_1 test_bank_2 \
			test_global_var test_global_array_size \
			test_struct_global test_class_global test_extern_global \
			test_max_depth

incorrect_dir=test_recursive_struct test_anonymous_enum test_parameter_exp \
			test_follow_call test_other_stmt test_empty_task

#FIXME:
#test_namespace test_c11_enum 
gen_ref: update
unit_test: diff
run: clean
	-rm -rf err.log
	-touch err.log
	-for name in `echo $(correct_dir)`; \
	do \
		cd $$name; sh run.sh; cd -; \
	done
	-for name in `echo $(incorrect_dir)`; \
	do \
		cd $$name; sh run.sh; cd -; \
	done

update: run
	-for name in $(correct_dir); \
	do \
		cp -f $$name/__merlin_kernel_list.h $$name/ref___merlin_kernel_list.h; \
		cd $$name; for one in `ls __merlinkernel_*.c*`; do mv  $$one  ref_$$one; done; cd -; \
		cp -f $$name/host.list $$name/ref_host.list; \
		cp -f $$name/kernel.list $$name/ref_kernel.list; \
		cp -f $$name/merlin_type_define.h $$name/ref_merlin_type_define.h; \
		if [ -e $$name/rose_vec_add.cpp ]; then \
		  cp -f $$name/rose_vec_add.cpp $$name/ref_rose_vec_add.cpp; \
		else \
		  cp -f $$name/rose_vec_add.c $$name/ref_rose_vec_add.c; \
		fi \
	done
	-for f in ${incorrect_dir}; \
	  do  \
	  cd $$f; cp critical_message.rpt critical_message.rpt.ref; cd -; \
	  done
      

diff: run
	-for name in $(correct_dir); \
	do \
		diff --brief $$name/__merlin_kernel_list.h $$name/ref___merlin_kernel_list.h &>> err.log; \
		cd $$name; for one in `ls __merlinkernel_*.c*`; do diff --brief  $$one  ref_$$one >>../err.log; done; cd -; \
		cd $$name; for one in `ls __merlinkernel_*.c*`; do gcc -fsyntax-only -std=c99 $$one 2> tmp.log || cat tmp.log >>../err.log; done; cd -; \
		diff --brief $$name/host.list $$name/ref_host.list &>> err.log; \
		diff --brief $$name/kernel.list $$name/ref_kernel.list &>> err.log; \
		diff --brief $$name/merlin_type_define.h $$name/ref_merlin_type_define.h &>> err.log; \
		if [ -e $$name/rose_vec_add.cpp ]; then \
		  diff --brief $$name/rose_vec_add.cpp $$name/ref_rose_vec_add.cpp &>> err.log; \
		else \
		  diff --brief $$name/rose_vec_add.c $$name/ref_rose_vec_add.c &>> err.log; \
		fi \
	done
	-for name in $(incorrect_dir); \
	do \
		grep -R "SEPAR" $$name/critical_message.rpt >> $$name/err.log; \
		if [ ! -s "$$name/err.log" ]; then \
			echo "$$name should report error" &>> err.log; \
		fi; \
	    diff $$name/critical_message.rpt $$name/critical_message.rpt.ref >> err.log; \
	done

clean:
	find . -name "rose*" -exec rm -rf {} \;
	find . -name "*.log" -exec rm -rf {} \;
	find . -name "host.list" -exec rm -rf {} \;
	find . -name "kernel.list" -exec rm -rf {} \;
	find . -name "*.rpt" -exec rm -rf {} \;
	find . -name "*.json" -exec rm -rf {} \;
	find . -name "__merlin*" -exec rm -rf {} \;
	find . -name "merlin_type_define.h" -exec rm -rf {} \;
	find . -name ".finish_read_files" -exec rm -rf {} \;
	rm -rf err.log
