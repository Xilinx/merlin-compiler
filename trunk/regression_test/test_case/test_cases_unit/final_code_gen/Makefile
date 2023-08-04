correct_dir=bank die port_no_const port_out_of_bound struct opencl_keywords const\
			assert uniquify global memcpy_in_wrapper extern register memcpy_in_func\
			global_novolatile global_2 const_2 add_hls_pragma_for_trival_statement \
			add_hls_pragma_for_scalar_only add_hls_pragma_for_empty_interface\
			uniquify_with_sub_kernel enum set_unique_function_name set_unique_function_name_2	\
			multi_kernel_prefetch attribute_pragma explicit_bundle_bundle auto_kernel	\
			return_pointer global_pntr multi_level_func_call_conflict check_arg_number_intel \
			func_name_too_long disable_burst_transform_pure_kernel \
			ap_int_fixed  add_label uniquify_scalar_address get_unique_wrapper_variable \
			check_conflict_kernel_name check_conflict_kernel_name2 bundle_testing bundle_testing_2 \
			migrate_mode hls_no_bundle
incorrect_dir=global_head no_wrapper check_arg_number_xilinx
gen_ref: update
unit_test: $(patsubst %,diff-%,$(correct_dir)) $(patsubst %,diff-f-%,$(incorrect_dir))
	cat $(patsubst %,%/err.log,$(correct_dir)) $(patsubst %,%/err.log,$(incorrect_dir)) > err.log
run-%: %  
	-cd $^; rm -rf err.log; sh run.sh 2>tmp.log || (cat tmp.log >> err.log)
run-f-%: %
	-cd $^; rm -rf err.log; touch err.log; sh run.sh
diff-%: run-%
	$(eval $@name := $(patsubst diff-%,%,$@))
	cd ${$@name}; for one in `ls rose_*.c*`; do diff -du  $$one  ref_$$one >>err.log; done; cd -; \
	cd ${$@name}; for one in `ls __merlin_parameter*`; do diff -du  $$one  ref_$$one >>err.log; done; cd -; \
	cd ${$@name}; for one in `ls __merlin_opencl_kernel_buffer*`; do diff -du  $$one  ref_$$one >>err.log; done; cd -; \
	if [ -s "${$@name}/err.log" ]; then \
		echo "${$@name} fail" >> ${$@name}/err.log; \
	fi
diff-f-%: run-%
	$(eval $@name := $(patsubst diff-f-%,%,$@))
	if [ -s "${$@name}/rose.log" ]; then \
		echo "${$@name} should report error" > ${$@name}/err.log; \
	fi 
update:  $(patsubst %,update-%,$(correct_dir))
update-%: run-%
	$(eval $@name := $(patsubst update-%,%,$@))
	cd ${$@name}; for one in `ls rose_*.c*`; do cp  $$one  ref_$$one; done; cd -; \
	cd ${$@name}; for one in `ls __merlin_parameter* `; do cp  $$one  ref_$$one; done; cd -; \
	cd ${$@name}; for one in `ls __merlin_opencl_kernel_buffer*`; do cp  $$one  ref_$$one; done; cd -
	
clean:
	find . -name "rose*" -exec rm -rf {} \;
	find . -name "log" -exec rm -rf {} \;
	find . -name "*.log" -exec rm -rf {} \;
	find . -name "host.list" -exec rm -rf {} \;
	find . -name "kernel.list" -exec rm -rf {} \;
	find . -name "*.rpt" -exec rm -rf {} \;
	find . -name "*.json" -exec rm -rf {} \;
	find . -name "flex*" -exec rm -rf {} \;
	find . -name "kernel_die.h" -exec rm -rf {} \;
	find . -name "__merlin_kernel_list.h" -exec rm -rf {} \;
	find . -name "critical_message.rpt.tt" -exec rm -rf {} \;
	find . -name "__merlin_api_list.c" -exec rm -rf {} \;
	find . -name ".finish_read_files" -exec rm -rf {} \;
	find . -name "__merlin_opencl*" -exec rm -rf {} \;
	find . -name "__merlin_parameter*" -exec rm -rf {} \;
	find . -name "ref___merlinkernel*" -exec rm -rf {} \;
	find . -name "channel_def.h" -exec rm -rf {} \;
	rm -rf err.log
