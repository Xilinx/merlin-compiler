sdaccel_cases = test test1 test_threshold test4 test_pattern test_pattern1 test_invalidpragma test_vendorpragma \
								test_call1 test_call2 test_call3 test_call4 test_call5 test_call6 \
				test_mod test_cond test_false_dependence_sdaccel test_auto_false_dependence \
				test_dual test_priority test_index_transform test_index_transform1 test_index_transform2 \
				test_factor_propagate test_assert test_alias test_alias1 test_flatten_off test_flatten_on \
				test_struct test_struct_2 test_struct_3 test_heuristic test_heuristic_func test_heuristic_const \
				test_heuristic_invariant test_global test_scope test_noncanonical test_err test_ap_int \
				test_partition_off  test_scope_range test_while test_visited_loop test_hls_unroll test_default_II \
				test_function_scope
aocl_cases = test test1 test_threshold test4 test_pattern test_call1 \
				test_call2 test_call3 test_call4 test_mod test_cond test_false_dependence_aocl \
				test_factor_propagate test_assert \
				test_noncanonical 
aocl_bug = test_alias
diff: $(patsubst %,diff-aocl-%,$(aocl_cases)) $(patsubst %,diff-sdaccel-%,$(sdaccel_cases)) 
	cat $(patsubst %,run_aocl_%/err.log,$(aocl_cases)) $(patsubst %,run_sdaccel_%/err.log,$(sdaccel_cases)) > err.log
	cat $(patsubst %,err_aocl_%.log,$(aocl_cases)) $(patsubst %,err_sdaccel_%.log,$(sdaccel_cases)) >> err.log
unit_test:
	-rm err.log
	make diff


diff-aocl-%: run-aocl-%
	$(eval $@case := $(patsubst diff-aocl-%,%,$@))
	-diff --brief run_aocl_${$@case}/rose_${$@case}.cpp test_rule/aocl/rose_${$@case}.cpp.ref  &>>err_aocl_${$@case}.log; \
	if [ -f test_rule/aocl/${$@case}.rpt.ref ]; then diff --brief run_aocl_${$@case}/critical_message.rpt test_rule/aocl/${$@case}.rpt.ref &>>err_aocl_${$@case}.log; fi
run-aocl-test1:
	$(eval $@case := $(patsubst run-aocl-%,%,$@))
	-rm -rf run_aocl_${$@case}; mkdir run_aocl_${$@case}; cd run_aocl_${$@case}; \
	touch err.log; mars_opt ../regression_parallel/test1.cpp -e c -p loop_parallel -a impl_tool=aocl -a auto_reshape_threshold=64 2>tmp.log || (cat tmp.log >> err.log)
run-aocl-%: 
	$(eval $@case := $(patsubst run-aocl-%,%,$@))
	-rm -rf run_aocl_${$@case}; mkdir run_aocl_${$@case}; cd run_aocl_${$@case}; \
	touch err.log; mars_opt ../regression_parallel/${$@case}.cpp -e c -p loop_parallel -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
diff-sdaccel-%: run-sdaccel-%
	$(eval $@case := $(patsubst diff-sdaccel-%,%,$@))
	-diff --brief run_sdaccel_${$@case}/rose_${$@case}.cpp test_rule/sdaccel/rose_${$@case}.cpp.ref  &>>err_sdaccel_${$@case}.log; \
	if [ -f test_rule/sdaccel/${$@case}.rpt.ref ]; then diff --brief run_sdaccel_${$@case}/critical_message.rpt test_rule/sdaccel/${$@case}.rpt.ref &>>err_sdaccel_${$@case}.log; fi
run-sdaccel-%: 
	$(eval $@case := $(patsubst run-sdaccel-%,%,$@))
	-rm -rf run_sdaccel_${$@case}; mkdir run_sdaccel_${$@case}; cd run_sdaccel_${$@case}; \
	touch err.log; mars_opt ../regression_parallel/${$@case}.cpp -e c -p loop_parallel -a impl_tool=sdaccel -I ${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin 2>tmp.log || (cat tmp.log >> err.log); \
	cd ../



run-sdaccel-test_thresold:  
	$(eval $@case := $(patsubst run-sdaccel-%,%,$@))
	-rm -rf run_sdaccel_${$@case}; mkdir run_sdaccel_${$@case}; cd run_sdaccel_${$@case}; \
	touch err.log; mars_opt ../regression_parallel/test_threshold.cpp -e c -p loop_parallel -a impl_tool=sdaccel -a dual_port_mem=off 2>tmp.log || (cat tmp.log >> err.log)
run-sdaccel-test_false_dependence_sdaccel:	
	$(eval $@case := $(patsubst run-sdaccel-%,%,$@))
	-rm -rf run_sdaccel_${$@case}; mkdir run_sdaccel_${$@case}; cd run_sdaccel_${$@case}; \
    touch err.log; mars_opt ../regression_parallel/test_false_dependence_sdaccel.cpp -e c -p loop_parallel -a impl_tool=sdaccel -a index_transform=off 2>tmp.log || (cat tmp.log >> err.log)
run-sdaccel-test_auto_false_dependence:	
	$(eval $@case := $(patsubst run-sdaccel-%,%,$@))
	-rm -rf run_sdaccel_${$@case}; mkdir run_sdaccel_${$@case}; cd run_sdaccel_${$@case}; \
	touch err.log; mars_opt ../regression_parallel/test_auto_false_dependence.cpp -e c -p loop_parallel -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	grep "fine_grained_opt_false_dependence" run_sdaccel_${$@case}/history_loop_parallel.json 2>tmp.log || (echo "False dep msg not found">> err.log && cat tmp.log >> err.log)
run-sdaccel-test_dual:	
	$(eval $@case := $(patsubst run-sdaccel-%,%,$@))
	-rm -rf run_sdaccel_${$@case}; mkdir run_sdaccel_${$@case}; cd run_sdaccel_${$@case}; \
	touch err.log; mars_opt ../regression_parallel/test_dual.cpp -e c -p loop_parallel -a dual_port_mem=off -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
run-sdaccel-test_priority:	
	$(eval $@case := $(patsubst run-sdaccel-%,%,$@))
	-rm -rf run_sdaccel_${$@case}; mkdir run_sdaccel_${$@case}; cd run_sdaccel_${$@case}; \
	touch err.log; mars_opt ../regression_parallel/test_priority.cpp -e c -p loop_parallel -a loop_flatten=on -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
run-sdaccel-test_index_transform1:	
	$(eval $@case := $(patsubst run-sdaccel-%,%,$@))
	-rm -rf run_sdaccel_${$@case}; mkdir run_sdaccel_${$@case}; cd run_sdaccel_${$@case}; \
	touch err.log; mars_opt ../regression_parallel/test_index_transform1.cpp -e c -p loop_parallel -a impl_tool=sdaccel -a index_transform=medium -a dual_port_mem=off 2>tmp.log || (cat tmp.log >> err.log)
run-sdaccel-test_index_transform2:	
	$(eval $@case := $(patsubst run-sdaccel-%,%,$@))
	-rm -rf run_sdaccel_${$@case}; mkdir run_sdaccel_${$@case}; cd run_sdaccel_${$@case}; \
	touch err.log; mars_opt ../regression_parallel/test_index_transform2.cpp -e c -p loop_parallel -a index_transform=medium -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
run-sdaccel-test_flatten_off:	
	$(eval $@case := $(patsubst run-sdaccel-%,%,$@))
	-rm -rf run_sdaccel_${$@case}; mkdir run_sdaccel_${$@case}; cd run_sdaccel_${$@case}; \
	touch err.log; mars_opt ../regression_parallel/test_flatten_off.cpp -e c -p loop_parallel -a loop_flatten=off -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
run-sdaccel-test_flatten_on:	
	$(eval $@case := $(patsubst run-sdaccel-%,%,$@))
	-rm -rf run_sdaccel_${$@case}; mkdir run_sdaccel_${$@case}; cd run_sdaccel_${$@case}; \
	touch err.log; mars_opt ../regression_parallel/test_flatten_on.cpp -e c -p loop_parallel -a loop_flatten=on -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
run-sdaccel-test_default_II:	
	$(eval $@case := $(patsubst run-sdaccel-%,%,$@))
	-rm -rf run_sdaccel_${$@case}; mkdir run_sdaccel_${$@case}; cd run_sdaccel_${$@case}; \
	touch err.log; mars_opt ../regression_parallel/test_default_II.cpp -e c -p loop_parallel -a impl_tool=sdaccel -a default_initiation_interval=1 2>tmp.log || (cat tmp.log >> err.log)  	

update: $(patsubst %,update-aocl-%,$(aocl_cases)) $(patsubst %,update-sdaccel-%,$(sdaccel_cases)) 
update-aocl-%: run-aocl-%
	$(eval $@case := $(patsubst update-aocl-%,%,$@))
	-cp run_aocl_${$@case}/rose_${$@case}.cpp test_rule/aocl/rose_${$@case}.cpp.ref; \
		if [ -f run_aocl_${$@case}/critical_message.rpt ]; then cp run_aocl_${$@case}/critical_message.rpt test_rule/aocl/${$@case}.rpt.ref; fi
update-sdaccel-%: run-sdaccel-%
	$(eval $@case := $(patsubst update-sdaccel-%,%,$@))
	-cp run_sdaccel_${$@case}/rose_${$@case}.cpp test_rule/sdaccel/rose_${$@case}.cpp.ref; \
		if [ -f run_sdaccel_${$@case}/critical_message.rpt ]; then cp run_sdaccel_${$@case}/critical_message.rpt test_rule/sdaccel/${$@case}.rpt.ref; fi
	
