
correct_dir=test_init_list multi-file-ctor class_in_header index_is_variable \
			template_class template_recursive_class \
			ap_int_size test_local_array_size template_class_template_member \
			test_typedef test_struct_with_class mer-1434 \
			inexplicit_call type_in_sub_header_file typedef_in_func return_type \
			typedef_in_function typedef_in_func_2 pragma_with_macro typedef_anonymous_class \
			variable_in_pragma test_inst_between_kernel_and_pragma mer-1862 \
            func_with_lower_type sizeof_in_header enum_in_function test_key_of_case_option \
	        test_condition_exp test_cxx_complex nested_class test_ap_shift_reg 

#FIXME
#template_const_static

incorrect_dir=kernel_in_namespace variable_array_in_class C_CPlusPlus_mix_use \
			  illegal_pragma template_recursive_function \
			  template_recursive_member_function undefined_func undefined_member_func \
	          typedef_in_extern_func
#FIXME
#test_negative_array_size
#test_undefined_func 

sub_tests=cxx_test

gen_ref: update 
unit_test: run 
run: $(patsubst %,run-correct-%,$(correct_dir)) $(patsubst %,run-incorrect-%,$(incorrect_dir)) \
	$(patsubst %,run-sub-tests-%,$(sub_tests)) 
	cat \
	  $(patsubst %,%/err.log,$(correct_dir)) \
	  $(patsubst %,%/err.log,$(incorrect_dir)) \
	  $(patsubst %,%/err.log,$(sub_tests)) \
		  > err.log
run-correct-%: %
	-cd $<; rm -rf err.log host.list kernel.list merlin_type_define.h __merlinkernel_*; sh run.sh; \
	diff -du __merlin_kernel_list.h ref___merlin_kernel_list.h &>> err.log; \
	for one in `ls __merlinkernel_*.c*`; do sed -i 's/__anonymous_0x[0-9a-f]*//' $$one; \
	diff -du  $$one  ref_$$one >> err.log; done; \
	for one in `ls __merlinkernel_*.c*`; do gcc -I ${MERLIN_COMPILER_HOME}/source-opt/include/apint_include -I. -fsyntax-only -std=c99 $$one 2> tmp.log || cat tmp.log >> err.log; done; \
	diff -du host.list ref_host.list &>> err.log; \
	diff -du kernel.list ref_kernel.list &>> err.log; \
	diff -du merlin_type_define.h ref_merlin_type_define.h &>> err.log; \
	if [ -s err.log ]; then echo $< failed >> err.log; fi

run-incorrect-%: %
	-cd $<; rm -rf err.log host.list kernel.list merlin_type_define.h __merlinkernel_*; sh run.sh; \
	grep -R "ERROR: " critical_message.rpt > tmp.log; \
	if [ ! -s tmp.log ]; then \
		echo "$< should report error" &>> err.log; \
	fi; \
	diff -du critical_message.rpt critical_message.rpt.ref &>> err.log; \
	if [ -s err.log ]; then echo $< failed >> err.log; fi


run-sub-tests-%:
	$(eval $@_folder := $(patsubst run-sub-tests-%,%,$@))
	make -C ${$@_folder} unit_test -j

update: 
	-for name in $(correct_dir); \
	do \
		cp -f $$name/__merlin_kernel_list.h $$name/ref___merlin_kernel_list.h; \
		cd $$name; for one in `ls __merlinkernel_*.c*`; do mv  $$one  ref_$$one; \
		sed -i 's/__anonymous_0x[0-9a-f]*//' ref_$$one; done; cd -; \
		cd $$name; for one in `ls rose_*.c*`; do mv  $$one  ref_$$one; \
		sed -i 's/__anonymous_0x[0-9a-f]*//' ref_$$one; done; cd -; \
		cp -f $$name/host.list $$name/ref_host.list; \
		cp -f $$name/kernel.list $$name/ref_kernel.list; \
		cp -f $$name/merlin_type_define.h $$name/ref_merlin_type_define.h; \
	done
	-for name in $(incorrect_dir); \
	do \
		cp $$name/critical_message.rpt $$name/critical_message.rpt.ref; \
	done
	-for name in $(sub_tests); \
	  do  \
	  	make -C $$name update; \
		done


clean: clean-sub-tests
	find . -name "rose*" -exec rm -rf {} \;
	find . -name "*.log" -exec rm -rf {} \;
	find . -name "host.list" -exec rm -rf {} \;
	find . -name "kernel.list" -exec rm -rf {} \;
	find . -name "*.rpt" -exec rm -rf {} \;
	find . -name "*.json" -exec rm -rf {} \;
	find . -name "__merlin*" -exec rm -rf {} \;
	find . -name "merlin*" -exec rm -rf {} \;
	find . -name ".finish_read_files" -exec rm -rf {} \;
	find . -name "*.ti" -exec rm -rf {} \;
	rm -rf err.log
