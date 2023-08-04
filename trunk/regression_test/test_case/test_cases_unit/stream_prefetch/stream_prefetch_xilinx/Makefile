cases=vec_add.cpp vec_add_fc_32.cpp  vec_add_full_prefetch.cpp \
	  vec_add_ap_int.cpp  vec_add_fc_4.cpp   vec_add_partial_prefetch.cpp \
	  multadd_constant_addr.cpp vec_add_with_streaming_access_attribute.cpp \
	  vec_add_non_standard_loop.cpp multadd_var_addr.cpp vec_add_cross_function.cpp \
	  vec_add_16_25_parallel.cpp vec_add_reference_sub_func.cpp multadd_constant_addr_incompatible.cpp \
	  vec_multMat_16.cpp vec_add_index_cross_function.cpp vec_add_data_reuse.cpp \
	  vec_add_data_reuse_with_intrinsic.cpp conv3d.cpp multadd_var_addr.cpp conv1d_short_length.cpp \
      multadd_var_addr_with_assert.cpp conv3d_with_streaming_intrinsic.cpp conv1d_extra_coloum_and_row.cpp \
	  vec_add_partial_unroll.cpp vec_add_exact_condition.cpp test_2d_condition.cpp    test_2d_condition_2.cpp \
	  vec_add_stream_off.cpp convolve_condition.cpp test_different_condition.cpp \
      vec_add_big_data_reuse.cpp vec_add_big_data_reuse_with_intrinsic.cpp \
	  vec_add_partial_access.cpp vec_add_hls_pragma.cpp
test: diff
	cat $(patsubst %,%.err.log,${cases}) > err.log
diff: $(patsubst %,diff-%,${cases})
diff-%: out-%
	$(eval $@_src_file := $(patsubst diff-%,%,$@))
	-f=`basename ${$@_src_file}`; gcc -fpermissive -fsyntax-only -I${MERLIN_COMPILER_HOME}/source-opt/include/apint_include \
	-I ${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin -std=c99 rose_${$@_src_file} 2>$@.tmp.log || cat $@.tmp.log >> ${$@_src_file}.err.log; \
	  diff -du $$f.out test_rule/$$f.ref &>> ${$@_src_file}.err.log
out-%: 
	$(eval $@_src_file := $(patsubst out-%,%,$@))
	rm -rf ${$@_src_file}.err.log
	touch ${$@_src_file}.err.log
	-base_f=`basename ${$@_src_file}`; \
	mars_opt ${$@_src_file} -e c -p stream_prefetch -a impl_tool=sdaccel -I ${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin 2>$@.tmp.log || (cat $@.tmp.log >> ${$@_src_file}.err.log); \
	cp rose_${$@_src_file} $${base_f}.out
update: $(patsubst %,update-%,${cases})
update-%:out-%
	$(eval $@_src_file := $(patsubst update-%,%,$@))
	-base_f=`basename ${$@_src_file}`; \
	cp $${base_f}.out test_rule/$${base_f}.ref
clean:
	rm -rf out-* *.log rose_* *.out critical_message.rpt history_stream_prefetch.json *.ti 
