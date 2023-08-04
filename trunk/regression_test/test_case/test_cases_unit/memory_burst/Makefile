cases=dwt.c   \
	denoise3D.c  \
	test2.c  \
	test_disable_burst.c  \
	test_burst_size.c   \
	test_unsupported_ref.c   \
	test_inbalanced_read_write.c   \
	test_burst_write_only.c   \
	test_lifting_efficiency.c   \
	test_burst_single_threshold.cpp   \
	test_large_buffer.c   \
	test_adjust_factor.c   \
	test_negative_offset.c   \
	test_iter_cross_func.c   \
    test_cast.cpp \
    test_void.cpp \
    test_data_reuse.c \
    test_complex_length.c \
    test_condition_write.cpp \
    test_single_access.cpp \
	test_data_reuse.c \
	test_partial_write.c \
	test_pointer_alias.c \
	test_pointer_alias_2.cpp \
	test_pointer_alias_3.cpp \
	test_pointer_alias_4.cpp \
	test_redundant_burst.cpp \
    test_indirect_index.c \
	test_complex_call_path.cpp  \
	test_negative_buffer_size.c \
	test_cache_basic.c \
	test_cache_hierarchy.c \
	denoise2D.c   \
	logistic.c   \
	twonn.cpp   \
	vec_add_kernel_lab1.c   \
	vec_add_kernel_lab2.c   \
	vec_add_kernel_lab3.c   \
	segmentation.c   \
	durbin.c   \
	durbin2.c   \
	matmul_kernel.c   \
	namd_kernel.c   \
	matmul_kernel.cpp \
	test_burst_with_manual_burst.c \
	test_pointer_cast.cpp \
	test_unbound.c \
	test_ap_int_fixed.cpp \
	test_class_type.cpp   \
    matmul_kernel_intel.cpp \
	test_burst_single_threshold_2.cpp \
	test_intel_flow_cg_parallel.c \
	test_xilinx_flow_cg_parallel.c \
    test_buff_name.cpp \
	test_false_negative.cpp \
    test_noncanonical.cpp \
	   test_struct.cpp  \
    test_var_in_scope.cpp  \
	test_var_in_scope_with_cache.cpp \
	test_short_burst_cg_parallel.cpp \
	test_long_burst_cg_parallel.cpp \
	test_scope_with_return.cpp \
	test_sdx_interface.cpp \
	test_sdx_interface_2.cpp \
	test_c_complex.c \
	test_cache_with_local_variable_address.c
hls_flow_case = test_hls_interface.cpp test_hls_interface_2.cpp
xilinx_case = test_write_only_in_dataflow_process.cpp
exe_cases = test_overflow_index
all_test:   build unit_test
gen_ref: update_ref
unit_test: run 
run: $(patsubst %,diff-%,$(cases)) $(patsubst %,diff-exe-%,$(exe_cases)) $(patsubst %,diff-x-%,$(xilinx_case)) $(patsubst %,diff-hls-%, $(hls_flow_case)) 
	cat $(patsubst %,%.err.log,$(cases) $(exe_cases) $(xilinx_case) $(hls_flow_case))> err.log
diff-hls-%: out-hls-%
	$(eval $@_src_file := $(patsubst diff-hls-%,%,$@))
	-f=`basename ${$@_src_file}`; gcc -fpermissive -fsyntax-only -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
	-std=c99 rose_${$@_src_file} 2>$@.tmp.log || cat $@.tmp.log >> ${$@_src_file}.err.log; \
	  diff -du $$f.sdaccel.out test_rule/$$f.sdaccel.ref &>> ${$@_src_file}.err.log 
out-hls-%: 
	$(eval $@_src_file := $(patsubst out-hls-%,%,$@))
	rm -rf ${$@_src_file}.err.log
	touch ${$@_src_file}.err.log
	-base_f=`basename ${$@_src_file}`; \
	mars_opt ${$@_src_file} -e c -p memory_burst -a impl_tool=sdaccel -a tool_version=vivado_hls 2>$@.tmp.log || (cat $@.tmp.log >> ${$@_src_file}.err.log); \
	cp rose_${$@_src_file} $${base_f}.sdaccel.out

diff-matmul_kernel_intel.cpp: out-matmul_kernel_intel.cpp
	$(eval $@_log_file := $(patsubst diff-%,%.err.log,$@))
	-diff -du  rose_matmul_kernel_intel.cpp test_rule/rose_matmul_kernel_intel.cpp &>>${$@_log_file}
	-gcc  -fsyntax-only -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
	-std=c99 rose_$(patsubst diff-%,%,$@) 2>$@.tmp.log || cat $@.tmp.log >> ${$@_log_file}
out-matmul_kernel_intel.cpp: 
	$(eval $@_log_file := $(patsubst out-%,%.err.log,$@))
	rm -rf ${$@_log_file}
	touch ${$@_log_file}
	-mars_opt matmul_kernel_intel.cpp  matmul_intel.cpp \
	  -e c -p memory_burst -a impl_tool=aocl -a effort=medium 2>$@.tmp.log || \
	  cat $@.tmp.log > ${$@_log_file}
diff-test_burst_single_threshold_2.cpp: out-test_burst_single_threshold_2.cpp
	$(eval $@_log_file := $(patsubst diff-%,%.err.log,$@))
	-diff -du  rose_test_burst_single_threshold_2.cpp test_rule/rose_test_burst_single_threshold_2.cpp &>>${$@_log_file}
	-gcc  -fsyntax-only -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
	-std=c99 rose_$(patsubst diff-%,%,$@) 2>$@.tmp.log || cat $@.tmp.log >> ${$@_log_file}
out-test_burst_single_threshold_2.cpp: 
	$(eval $@_log_file := $(patsubst out-%,%.err.log,$@))
	rm -rf ${$@_log_file}
	touch ${$@_log_file}
	-mars_opt test_burst_single_threshold_2.cpp  -a burst_single_size_threshold=4000000 \
	  -e c -p memory_burst -a impl_tool=sdaccel 2>$@.tmp.log \
	  || (cat $@.tmp.log > ${$@_log_file})
diff-test_intel_flow_cg_parallel.c: out-test_intel_flow_cg_parallel.c
	$(eval $@_log_file := $(patsubst diff-%,%.err.log,$@))
	-diff -du  rose_test_intel_flow_cg_parallel.c test_rule/rose_test_intel_flow_cg_parallel.c &>>${$@_log_file}
	-gcc  -fsyntax-only -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
	-std=c99 rose_$(patsubst diff-%,%,$@) 2>$@.tmp.log || cat $@.tmp.log >> ${$@_log_file}
out-test_intel_flow_cg_parallel.c: 
	$(eval $@_log_file := $(patsubst out-%,%.err.log,$@))
	rm -rf ${$@_log_file}
	touch ${$@_log_file}
	-mars_opt test_intel_flow_cg_parallel.c  \
	  -e c -p memory_burst -a impl_tool=aocl -a impl_tool=aocl -a effort=standard 2>$@.tmp.log \
	  || (cat $@.tmp.log > ${$@_log_file})
diff-test_xilinx_flow_cg_parallel.c: out-test_xilinx_flow_cg_parallel.c
	$(eval $@_log_file := $(patsubst diff-%,%.err.log,$@))
	-diff -du  rose_test_xilinx_flow_cg_parallel.c test_rule/rose_test_xilinx_flow_cg_parallel.c &>>${$@_log_file}
	-gcc  -fsyntax-only -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
	-std=c99 rose_$(patsubst diff-%,%,$@) 2>$@.tmp.log || cat $@.tmp.log >> ${$@_log_file}
out-test_xilinx_flow_cg_parallel.c: 
	$(eval $@_log_file := $(patsubst out-%,%.err.log,$@))
	rm -rf ${$@_log_file}
	touch ${$@_log_file}
	-mars_opt test_xilinx_flow_cg_parallel.c \
	  -e c -p memory_burst  -a impl_tool=sdaccel -a effort=standard 2>$@.tmp.log \
	  || (cat $@.tmp.log > ${$@_log_file})
diff-test_buff_name.cpp: out-test_buff_name.cpp
	$(eval $@_log_file := $(patsubst diff-%,%.err.log,$@))
	-diff -du  rose_test_buff_name.cpp test_rule/rose_test_buff_name.cpp &>>${$@_log_file}
	-gcc  -fsyntax-only -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
	-std=c99 rose_$(patsubst diff-%,%,$@) 2>$@.tmp.log || cat $@.tmp.log >> ${$@_log_file}
out-test_buff_name.cpp: 
	$(eval $@_log_file := $(patsubst out-%,%.err.log,$@))
	rm -rf ${$@_log_file}
	touch ${$@_log_file}
	-mars_opt test_buff_name.cpp  \
	  -e c -p memory_burst -a impl_tool=sdaccel 2>$@.tmp.log || \
	  cat $@.tmp.log > ${$@_log_file}

diff-%: out-%
	$(eval $@_src_file := $(patsubst diff-%,%,$@))
	-f=`basename ${$@_src_file}`; gcc -fpermissive -fsyntax-only -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
	-std=c99 rose_${$@_src_file} 2>$@.tmp.log || cat $@.tmp.log >> ${$@_src_file}.err.log; \
	  diff -du $$f.aocl.out test_rule/$$f.aocl.ref &>> ${$@_src_file}.err.log; \
	  diff -du $$f.aocl.high.out test_rule/$$f.aocl.high.ref &>> ${$@_src_file}.err.log; \
	  diff -du $$f.sdaccel.out test_rule/$$f.sdaccel.ref &>> ${$@_src_file}.err.log 
out-%: 
	$(eval $@_src_file := $(patsubst out-%,%,$@))
	rm -rf ${$@_src_file}.err.log
	touch ${$@_src_file}.err.log
	-base_f=`basename ${$@_src_file}`; \
	mars_opt ${$@_src_file} -e c -p memory_burst -a impl_tool=aocl 2>$@.tmp.log || (cat $@.tmp.log >> ${$@_src_file}.err.log); \
	cp rose_${$@_src_file} $${base_f}.aocl.out; \
	mars_opt ${$@_src_file} -e c -p memory_burst -a impl_tool=aocl -a effort=high 2>$@.tmp.log || (cat $@.tmp.log >> ${$@_src_file}.err.log); \
	cp rose_${$@_src_file} $${base_f}.aocl.high.out; \
	mars_opt ${$@_src_file} -e c -p memory_burst -a impl_tool=sdaccel 2>$@.tmp.log || (cat $@.tmp.log >> ${$@_src_file}.err.log); \
	cp rose_${$@_src_file} $${base_f}.sdaccel.out
diff-x-%: out-x-%
	$(eval $@_src_file := $(patsubst diff-x-%,%,$@))
	-f=`basename ${$@_src_file}`; gcc -fpermissive -fsyntax-only -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
	-std=c99 rose_${$@_src_file} 2>$@.tmp.log || cat $@.tmp.log >> ${$@_src_file}.err.log; \
	  diff -du $$f.sdaccel.out test_rule/$$f.sdaccel.ref &>> ${$@_src_file}.err.log 
out-x-%: 
	$(eval $@_src_file := $(patsubst out-x-%,%,$@))
	rm -rf ${$@_src_file}.err.log
	touch ${$@_src_file}.err.log
	-base_f=`basename ${$@_src_file}`; \
	mars_opt ${$@_src_file} -e c -p memory_burst -a impl_tool=sdaccel 2>$@.tmp.log || (cat $@.tmp.log >> ${$@_src_file}.err.log); \
	cp rose_${$@_src_file} $${base_f}.sdaccel.out

update_ref: $(patsubst %,update-%,$(cases)) $(patsubst %,update-x-%,$(xilinx_case)) $(patsubst %,update-hls-%,$(hls_flow_case))
update-%: out-%
	$(eval $@_src_file := $(patsubst update-%,%,$@))
	f=`basename ${$@_src_file}`; \
	cp $$f.aocl.out test_rule/$$f.aocl.ref; \
	cp $$f.aocl.high.out test_rule/$$f.aocl.high.ref; \
	cp $$f.sdaccel.out test_rule/$$f.sdaccel.ref
update-x-%: out-x-%
	$(eval $@_src_file := $(patsubst update-x-%,%,$@))
	f=`basename ${$@_src_file}`; \
	cp $$f.sdaccel.out test_rule/$$f.sdaccel.ref
update-hls-%: out-hls-%
	$(eval $@_src_file := $(patsubst update-hls-%,%,$@))
	f=`basename ${$@_src_file}`; \
	cp $$f.sdaccel.out test_rule/$$f.sdaccel.ref

update-test_burst_single_threshold_2.cpp: out-test_burst_single_threshold_2.cpp
	-cp -f rose_test_burst_single_threshold_2.cpp test_rule/rose_test_burst_single_threshold_2.cpp 
update-matmul_kernel_intel.cpp: out-matmul_kernel_intel.cpp
	-cp -f rose_matmul_kernel_intel.cpp test_rule/rose_matmul_kernel_intel.cpp 
update-test_intel_flow_cg_parallel.c: out-test_intel_flow_cg_parallel.c
	-cp -f rose_test_intel_flow_cg_parallel.c test_rule/rose_test_intel_flow_cg_parallel.c 
update-test_xilinx_flow_cg_parallel.c: out-test_xilinx_flow_cg_parallel.c
	-cp -f rose_test_xilinx_flow_cg_parallel.c test_rule/rose_test_xilinx_flow_cg_parallel.c
update-test_buff_name.cpp: out-test_buff_name.cpp
	-cp -f rose_test_buff_name.cpp test_rule/rose_test_buff_name.cpp


diff-exe-%: 
	$(eval $@_src_file := $(patsubst diff-exe-%,%,$@))
	-rm -rf ${$@_src_file}.err.log; touch ${$@_src_file}.err.log; \
	mars_opt ${$@_src_file}.cpp -e c -p memory_burst; \
	g++ rose_${$@_src_file}.cpp -o rose_${$@_src_file}; ./rose_${$@_src_file} \
	|| echo "${$@_src_file} fails in simulation" > ${$@_src_file}.err.log
   
clean:
	rm -rf rose* *.o
