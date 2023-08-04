all_test:   build unit_test
gen_ref: update
unit_test: run 
supported_cases = test_field_name_as_depth.c \
			   test.cpp \
	test_pragma.cpp \
	test_pragma_2.cpp \
	test_pragma_3.cpp \
	test_pragma_4.cpp \
	test2.cpp \
	test3.cpp \
	test_complex_assign.cpp \
	test_access_type.c \
	test_access_type2.c \
	test_access_type3.c \
	test_access_type3.cpp \
	test_access_type4.c \
	test_access_type5.c \
	test_access_type6.c \
	test_address_of_array.c \
	test_const_access.cpp \
	test_trival_pointer_cast.cpp \
	test_trival_pointer_cast.c \
	test_alias.c \
	test_alias2.c \
	test_alias3.c \
	test_array.c \
	test_array2.c \
	test_array3.c \
	test_array4.c \
	test_array_member.c \
	test_call_graph.c \
	test_call_graph2.c \
	test_call_exp.c \
	test_class_member.cpp \
	test_global.c \
	test_global.cpp \
	test_global2.c \
	test_initializer.c \
	test_local.c \
	test_local.cpp \
	test_local_mutable.cpp \
	test_aggr_init.c \
	test_memcpy.c \
	test_operation.c \
	test_pointer_member.c \
	test_pointer_member.cpp \
	test_reference.cpp \
	test_struct_member.c \
	test_struct_return.c \
	test_dead_remove.c \
	test_dead_remove_MER_1764.cpp \
	test_dead_remove_with_multi_calls.cpp \
	test_unused_pragma.c \
	test_nullptr.cpp \
	test_union.c \
	test_union2.c \
	test_empty_struct.cpp \
	test_type.cpp \
	test_return_pointer_struct.c \
	test_assign_reference.cpp \
	test_multiple_dim.c \
	test_array_empty_size.c \
	test_depth_attribute.c \
	test_unused_field.cpp \
	test_volatile_access.cpp \
	test_pointer_operation.c \
	test_call_as_argument.c \
	test_side_effect.c  \
	anonymous_enum_data_member.c \
	test_typedef.c \
	test_typedef2.c \
	anonymous_union.cpp \
					test_unsupported_global.c \
	test_struct_with_ap_int.cpp \
	test_multi_interface_pragma.c \
    test_global_argument.cpp \
    test_const_cast.c \
	test_cast.cpp \
	test_pointer_op_as_argument.cpp \
    test_anon_strut_member.c \
    test_global_input_output.cpp \
	test_ap_int.cpp
unsupported_cases = \
	test_pointer_cast.cpp \
	test_pointer_cast1.cpp \
	test_pointer_cast2.cpp \
	test_pointer_cast_argument.cpp \
	test_unsupported_cast.c \
					test_recursive.c \
					test_unsupported_depth.c \
					test_head_file.c \
					test_mem_call.cpp \
	test_missing_depth.c \
	test_incorrect_depth.c \
	test_bitfield.c \
	test_unsupported_type.cpp \
	test_missing_depth_with_multi_kernel.c \
	test_initializer_in_for_init.cpp \
	test_partial_decompose.cpp
aligned_struct_decomp_cases = \
					   test_access_type6_aligned_struct_decomp.c
hls_cases = test_dead_ports.c 
#unsupported_initializer.c // crash 
run: $(patsubst %,diff-%,${supported_cases}) $(patsubst %,diff-f-%,${unsupported_cases}) diff-f-test_many_fields.c \
	$(patsubst %,diff-force-%,${aligned_struct_decomp_cases}) $(patsubst %, diff-hls-%, $(hls_cases)) \
	diff-pure-kernel-test_assign_in_pure_kernel.cpp
	cat $(patsubst %,%.err.log,${supported_cases} ${unsupported_cases} ${aligned_struct_decomp_cases} test_many_fields.c test_assign_in_pure_kernel.cpp) >err.log

diff-pure-kernel-%: out-pure-kernel-%
	$(eval $@_src_file := $(patsubst diff-pure-kernel-%,%,$@))
	  -gcc -std=c99 -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
				  -fsyntax-only rose_${$@_src_file} 2>tmp.log || (cat tmp.log >> ${$@_src_file}.err.log)
	  -diff --brief  rose_${$@_src_file} test_rule/rose_${$@_src_file} &>>${$@_src_file}.err.log
	
out-pure-kernel-%: %
	rm -rf $^.err.log
	 -mars_opt $^ -e c -p struct_decompose -a impl_tool=sdaccel -a pure_kernel=on 2>tmp.log || (cat tmp.log >> $^.err.log)
	
out-f-test_many_fields.c: test_many_fields.c
	-rm -rf run-$^; mkdir -p run-$^; cd run-$^; mars_opt ../$^ -e c -p struct_decompose -a impl_tool=aocl; 
diff-%: out-% 
	$(eval $@_src_file := $(patsubst diff-%,%,$@))
	  -gcc -std=c99 -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
				  -fsyntax-only rose_${$@_src_file} 2>tmp.log || (cat tmp.log >> ${$@_src_file}.err.log)
	  -diff --brief  rose_${$@_src_file} test_rule/rose_${$@_src_file} &>>${$@_src_file}.err.log
diff-f-%: out-f-%
	$(eval $@_src_file := $(patsubst diff-f-%,%,$@))
	-diff --brief run-${$@_src_file}/critical_message.rpt \
	  test_rule/critical_message_${$@_src_file}.ref &>> ${$@_src_file}.err.log
diff-force-%: out-force-% 
	$(eval $@_src_file := $(patsubst diff-force-%,%,$@))
	  -gcc -std=c99 -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
				  -fsyntax-only rose_${$@_src_file} 2>tmp.log || (cat tmp.log >> ${$@_src_file}.err.log)
	  -diff --brief  rose_${$@_src_file} test_rule/rose_${$@_src_file} &>>${$@_src_file}.err.log

out-force-%: %
	rm -rf $^.err.log
	 -mars_opt $^ -e c -p struct_decompose -a impl_tool=sdaccel -a aligned_struct_decomp=off 2>tmp.log || (cat tmp.log >> $^.err.log)
out-%: %
	rm -rf $^.err.log
	 -mars_opt $^ -e c -p struct_decompose -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> $^.err.log)
out-f-%: %
	rm -rf $^.err.log
	-mkdir -p run-$^; cd run-$^; mars_opt ../$^ -e c -p struct_decompose -a impl_tool=sdaccel; 


diff-hls-%: out-hls-% 
	$(eval $@_src_file := $(patsubst diff-hls-%,%,$@))
	  -gcc -std=c99 -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
				  -fsyntax-only rose_${$@_src_file} 2>tmp.log || (cat tmp.log >> ${$@_src_file}.err.log)
	  -diff --brief  rose_${$@_src_file} test_rule/rose_${$@_src_file} &>>${$@_src_file}.err.log
out-hls-%: %
	rm -rf $^.err.log
	 -mars_opt $^ -e c -p struct_decompose -a impl_tool=sdaccel -a tool_version=vitis_hls 2>tmp.log || (cat tmp.log >> $^.err.log)

#   -mars_opt test_local2.cpp -e c -p struct_decompose 2>tmp.log || (cat tmp.log >> err.log)
#	-mars_opt test_stl_member.cpp -e c -p struct_decompose 2>tmp.log || (cat tmp.log >> err.log)

update: $(patsubst %,update-%,${supported_cases}) $(patsubst %,update-f-%,${unsupported_cases}) \
	update-f-test_many_fields.c  update-pure-kernel-test_assign_in_pure_kernel.cpp $(patsubst %,update-hls-%, $(hls_cases)) \
	$(patsubst %,update-force-%,${aligned_struct_decomp_cases})
update-%: out-%
	$(eval $@_src_file := $(patsubst update-%,%,$@))
	-cp rose_${$@_src_file} test_rule/rose_${$@_src_file}
update-hls-%: out-hls-%
	$(eval $@_src_file := $(patsubst update-hls-%,%,$@))
	-cp rose_${$@_src_file} test_rule/rose_${$@_src_file}
update-force-%: out-force-%
	$(eval $@_src_file := $(patsubst update-force-%,%,$@))
	-cp rose_${$@_src_file} test_rule/rose_${$@_src_file}
update-f-%: out-f-%
	$(eval $@_src_file := $(patsubst update-f-%,%,$@))
	-cp run-${$@_src_file}/critical_message.rpt test_rule/critical_message_${$@_src_file}.ref
update-pure-kernel-%:out-pure-kernel-%
	$(eval $@_src_file := $(patsubst update-pure-kernel-%,%,$@))
	-cp rose_${$@_src_file} test_rule/rose_${$@_src_file}

clean:
	rm -rf rose* *.o
