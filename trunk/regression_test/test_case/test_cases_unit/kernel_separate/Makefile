correct_dir=test_basic test_sub_function_call test_global_var test_struct \
			test_struct_global test_class test_class_global test_typedef \
			test_sub_function_call_multi test_sub_function_call_multi2 \
			test_struct_multi test_class_multi test_typedef_multi \
			test_cross test_recursive_typedef test_system_function_call \
			test_blackschole test_typedef_builtin test_struct_ref \
			test_extern_global test_enum_cast test_typedef_include\
			test_enum_val test_enum_sizeof test_call_in_global_init \
			test_asm_stmt test_system_type \
			test_directive_in_func_body test_header_file test_global_array_size \
			test_system_typedef test_call_graph test_depth_pragma test_merlin_stream \
			test_sub_function_call_multi_level \
			test_class_function test_class_inline test_class_pragma test_struct_with_enum \
			test_static_func_in_header test_depth_attribute test_multiple_typedef test_multiple_typedef_2 \
			test_shared_sub_func test_template_class test_hls_stream_with_typedef \
			test_typename test_template_parameter test_typedef_dep \
			test_ap_int test_inline_class test_fprintf test_call_template test_enum_global \
			test_fake_anonymous test_floating_decl test_static_assert test_compound_literal_exp \
			test_compound_literal_with_enum test_complex test_recursive_host \
			test_static_functions_with_same_signature test_static_functions_with_same_signature2 \
            test_type_trait_builtin_operator test_empty_array_size test_inline_enum \
	        test_typedef_in_func test_static_functions_with_same_signature3 test_typedef_anonymous \
			test_standalone_type test_use_inner_type_only test_pseudodestructor test_read_only_static_variable \
			floating_var_ref kernel_pragma_in_forward_decl kernel_pragma_in_forward_decl_2 \
			test_static_functions_with_same_signature4 global_variable_rename \
			global_symbol_rename global_symbol_rename2 test_enum_used_only_in_pragma test_anonymous_type \
			test_static_variable test_recover_hls_prefix
incorrect_dir=test_recursive_struct test_anonymous_enum test_recursive_global \
			  test_shared_global_and_static test_merlin_stream_rw \
			  test_extern_const_global test_function_pointer test_undefined_class \
			  test_system_struct test_typedef_pointer_to_anonymous_struct test_static_kernel
#FIXME:test_template_const_static
gen_ref: update
unit_test: run 
run: $(patsubst %,run-correct-%,$(correct_dir)) $(patsubst %,run-incorrect-%,$(incorrect_dir))
	cat \
	  $(patsubst %,%/err.log,$(correct_dir)) \
	  $(patsubst %,%/err.log,$(incorrect_dir)) \
		  > err.log
run-correct-%: %
	-cd $<; rm -rf err.log host.list kernel.list merlin_type_define.h __merlinkernel_*; sh run.sh; \
	diff -du __merlin_kernel_list.h ref___merlin_kernel_list.h &>> err.log; \
	for one in `ls __merlinkernel_*.c*`; do diff -du  $$one  ref_$$one >> err.log; done; \
	for one in `ls __merlinkernel_*.c*`; do gcc -I ${MERLIN_COMPILER_HOME}/source-opt/include/apint_include -I. -fsyntax-only -std=c99 $$one 2> tmp.log || cat tmp.log >> err.log; done; \
	diff -du host.list ref_host.list &>> err.log; \
	diff -du kernel.list ref_kernel.list &>> err.log; \
	diff -du merlin_type_define.h ref_merlin_type_define.h &>> err.log; \
	if [ -s err.log ]; then echo $< failed >> err.log; fi

run-incorrect-%: %
	-cd $<; rm -rf err.log host.list kernel.list merlin_type_define.h __merlinkernel_*; sh run.sh; \
	grep -RE "ERROR|WARNING" critical_message.rpt > tmp.log; \
	if [ ! -s tmp.log ]; then \
		echo "$< should report error" &>> err.log; \
	fi; \
	diff -du critical_message.rpt critical_message.rpt.ref &>> err.log; \
	if [ -s err.log ]; then echo $< failed >> err.log; fi


update: run 
	-for name in $(correct_dir); \
	do \
		cp -f $$name/__merlin_kernel_list.h $$name/ref___merlin_kernel_list.h; \
		cd $$name; for one in `ls __merlinkernel_*.c*`; do mv  $$one  ref_$$one; done; cd -; \
		cp -f $$name/host.list $$name/ref_host.list; \
		cp -f $$name/kernel.list $$name/ref_kernel.list; \
		cp -f $$name/merlin_type_define.h $$name/ref_merlin_type_define.h; \
	done
	-for f in ${incorrect_dir}; \
	  do  \
	  cd $$f; cp critical_message.rpt critical_message.rpt.ref; cd -; \
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
	find . -name "critical_message.rpt" -exec rm -rf {} \;
	rm -rf err.log
