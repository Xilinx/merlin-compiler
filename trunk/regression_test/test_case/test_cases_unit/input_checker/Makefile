
unit_test: diff
run: clean 
	-mars_opt regression_inputcheck/check_kernel_name/test.c -e c -p synthesis_check -a impl_tool=sdaccel -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_kernel_name.rpt
	-cd regression_inputcheck/embed_define_macro && mars_opt -DF=16 -DPF=4 embed_define_macro.cpp -e c -p synthesis_check -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_inputcheck/addressof_op/test_addressof_op_err.c -e c -p synthesis_check
	-mv critical_message.rpt critical_message_addr.rpt
	-mars_opt regression_inputcheck/unsupported_type/test_unsupported_type.c -e c -p synthesis_check -a pure_kernel=on -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_unsupported_type.rpt
	-mars_opt regression_inputcheck/check_opencl_conflict/test1.cpp -e c -p synthesis_check -a impl_tool=aocl -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_opencl_conflict.rpt
	-mars_opt regression_inputcheck/check_opencl_conflict/test2.cpp -e c -p synthesis_check -a impl_tool=sdaccel -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_opencl_conflict_xilinx.rpt
	-mars_opt regression_inputcheck/check_arg_number_xilinx/test1.cpp -e c -p synthesis_check 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_arg_number_xilinx.rpt
	-mars_opt regression_inputcheck/enum_arg_check/test.c -e c -p synthesis_check -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_enum.rpt
	-mars_opt regression_inputcheck/enum_arg_check/test2.c -e c -p synthesis_check -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_enum2.rpt
	-mars_opt regression_inputcheck/check_array_type/check_array_type1.c -e c -p synthesis_check -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_array_type1.rpt
	-mars_opt regression_inputcheck/check_array_type/check_array_type2.c -e c -p synthesis_check -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_inputcheck/pragma/pragma_invalid.cpp -e c -p synthesis_check -a impl_tool=sdaccel -a pure_kernel=on  2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_pragma.rpt
	-mars_opt regression_inputcheck/hls_stream/test_hls_stream_value.cpp -e c -p synthesis_check -a impl_tool=sdaccel   2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_value_hls_stream.rpt
	-mars_opt regression_inputcheck/hls_stream/test_hls_stream_value.cpp -e c -p synthesis_check -a impl_tool=sdaccel -a pure_kernel=on  2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_value_hls_stream_pure_kernel.rpt
	-mars_opt regression_inputcheck/hls_stream/test_hls_stream_reference.cpp -e c -p synthesis_check -a impl_tool=sdaccel  2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_reference_hls_stream.rpt
	touch critical_message.rpt;
	-mars_opt regression_inputcheck/hls_stream/test_hls_stream_reference.cpp -e c -p synthesis_check -a impl_tool=sdaccel -a pure_kernel=on  2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_reference_hls_stream_pure_kernel.rpt
	-gcc -fsyntax-only -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
	  regression_inputcheck/hls_stream/test_hls_stream_value_sub_func.cpp 2>tmp.log; \
	  grep "error:" tmp.log || echo fail to catch invalid hls::stream use >> err.log
	touch critical_message.rpt;
	-mars_opt regression_inputcheck/hls_stream/test_hls_stream_reference_sub_func.cpp -e c -p synthesis_check -a impl_tool=sdaccel -a pure_kernel=on  2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_hls_stream_sub_func_pure_kernel.rpt
	touch critical_message.rpt;
	-mars_opt regression_inputcheck/hls_stream/test_hls_stream_reference_sub_func.cpp -e c -p synthesis_check -a impl_tool=sdaccel  2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_hls_stream_sub_func.rpt
	touch critical_message.rpt;
	-mars_opt regression_inputcheck/test_as_top/incorrect_system_func.cpp -e c -p synthesis_check -a impl_tool=sdaccel  2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_system_func.rpt
	touch critical_message.rpt;
	-mars_opt regression_inputcheck/test_reference_type/test_reference.cpp -e c -p synthesis_check -a impl_tool=sdaccel  -a tool_version=vitis_hls 2>tmp.log || (cat tmp.log >> err.log)
	-mv critical_message.rpt critical_message_check_reference_port.rpt
gen_ref: update

update:run
	-cp critical_message_check_kernel_name.rpt test_rule/critical_message_check_kernel_name.rpt
	-cp regression_inputcheck/embed_define_macro/rose_embed_define_macro.cpp test_rule/test_embed_define_macro.out.ref 
	-cp critical_message_addr.rpt test_rule/msg_critical_message_addr.ref
	-cp critical_message_unsupported_type.rpt test_rule/msg_critical_message_unsupported_type.ref 
	-cp critical_message_check_opencl_conflict.rpt test_rule/msg_critical_message_check_opencl_conflict.rpt
	-cp critical_message_check_opencl_conflict_xilinx.rpt test_rule/msg_critical_message_check_opencl_conflict_xilinx.rpt
	-cp critical_message_check_arg_number_xilinx.rpt test_rule/critical_message_check_arg_number_xilinx.rpt
	-cp critical_message_check_enum.rpt test_rule/critical_message_check_enum.rpt
	-cp critical_message_check_enum2.rpt test_rule/critical_message_check_enum2.rpt
	-cp critical_message_check_array_type1.rpt test_rule/critical_message_check_array_type1.rpt
	-cp critical_message_check_pragma.rpt test_rule/critical_message_check_pragma.rpt
	-cp rose_check_array_type2.c test_rule/check_array_type2.ref
	-cp critical_message_check_value_hls_stream.rpt test_rule/critical_message_check_value_hls_stream.ref
	-cp critical_message_check_value_hls_stream_pure_kernel.rpt test_rule/critical_message_check_value_hls_stream_pure_kernel.ref
	-cp critical_message_check_reference_hls_stream.rpt test_rule/critical_message_check_reference_hls_stream.ref
	-cp critical_message_check_reference_hls_stream_pure_kernel.rpt test_rule/critical_message_check_reference_hls_stream_pure_kernel.ref
	-cp critical_message_check_hls_stream_sub_func_pure_kernel.rpt test_rule/critical_message_check_hls_stream_sub_func_pure_kernel.ref
	-cp critical_message_check_hls_stream_sub_func.rpt test_rule/critical_message_check_hls_stream_sub_func.ref
	-cp critical_message_check_system_func.rpt test_rule/critical_message_check_system_func.ref
	-cp critical_message_check_reference_port.rpt test_rule/critical_message_check_reference_port.ref

diff: run
	-diff --brief critical_message_check_kernel_name.rpt test_rule/critical_message_check_kernel_name.rpt &>> err.log
	-diff --brief regression_inputcheck/embed_define_macro/rose_embed_define_macro.cpp test_rule/test_embed_define_macro.out.ref &>> err.log
	-diff --brief critical_message_addr.rpt test_rule/msg_critical_message_addr.ref &>> err.log
	-diff --brief  critical_message_unsupported_type.rpt test_rule/msg_critical_message_unsupported_type.ref &>>err.log
	-diff --brief critical_message_check_opencl_conflict.rpt test_rule/msg_critical_message_check_opencl_conflict.rpt &>>err.log
	-diff --brief critical_message_check_opencl_conflict_xilinx.rpt test_rule/msg_critical_message_check_opencl_conflict_xilinx.rpt &>>err.log
	-diff --brief critical_message_check_arg_number_xilinx.rpt test_rule/critical_message_check_arg_number_xilinx.rpt &>>err.log
	-diff --brief critical_message_check_enum.rpt test_rule/critical_message_check_enum.rpt &>>err.log
	-diff --brief critical_message_check_enum2.rpt test_rule/critical_message_check_enum2.rpt &>>err.log
	-diff --brief critical_message_check_array_type1.rpt test_rule/critical_message_check_array_type1.rpt &>> err.log
	-diff --brief critical_message_check_pragma.rpt test_rule/critical_message_check_pragma.rpt &>>err.log
	-diff --brief rose_check_array_type2.c test_rule/check_array_type2.ref &>> err.log
	-diff --brief critical_message_check_value_hls_stream.rpt test_rule/critical_message_check_value_hls_stream.ref &>>err.log
	-diff --brief critical_message_check_value_hls_stream_pure_kernel.rpt test_rule/critical_message_check_value_hls_stream_pure_kernel.ref &>>err.log
	-diff --brief critical_message_check_reference_hls_stream.rpt test_rule/critical_message_check_reference_hls_stream.ref &>>err.log
	-diff --brief critical_message_check_reference_hls_stream_pure_kernel.rpt test_rule/critical_message_check_reference_hls_stream_pure_kernel.ref &>>err.log
	-diff --brief critical_message_check_hls_stream_sub_func_pure_kernel.rpt test_rule/critical_message_check_hls_stream_sub_func_pure_kernel.ref &>>err.log
	-diff --brief critical_message_check_hls_stream_sub_func.rpt test_rule/critical_message_check_hls_stream_sub_func.ref &>>err.log
	-diff --brief critical_message_check_system_func.rpt test_rule/critical_message_check_system_func.ref &>> err.log
	-diff --brief critical_message_check_reference_port.rpt test_rule/critical_message_check_reference_port.ref &>> err.log

clean:
	-rm -rf regression_inputcheck/*/*.log
	-rm -rf regression_inputcheck/*/*.o
	-rm -rf regression_inputcheck/*/rose*
	-rm -rf regression_inputcheck/*/*.rpt
	-rm -rf regression_inputcheck/*/*.h
	-rm -rf regression_inputcheck/*/*.json
	-rm -rf regression_inputcheck/*/*.ti
	-rm -rf regression_inputcheck/*/.finish_read_files
	-rm -rf *.log *.o rose* *.rpt *.h *.json *.ti .finish_read_files
