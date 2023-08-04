exe_cases = test_1d_stencil_sim \
			test_1d_array_sim \
			test_2d_sim \
			test_2d_range_sim \
			test_2d_struct_sim \
			test_pointer_sim \
			test_parallel_sim \
			test_multi_unroll_loops_sim \
			test_1x1_sim \
			test_5x1_sim \
			test_condition_sim \
      test_negative_loop_sim \
			test_3d_sim 
cases = conv1d4x4.cpp  \
		test_while.cpp \
		test_2d_condition.cpp\
        conv1d2x2.cpp    \
    test_negative_loop_coeff.cpp \
		test_level.cpp  \
		test_partial_stencil.cpp \
		test_multi_unroll_loops.cpp \
        conv1d2x5.cpp  \
		test_multi_variable_diff.cpp \
		test_multi_variable_same.cpp \
		test_pointer.cpp   \
		test_scope.cpp \
		test_interface.cpp \
		test_bound.cpp \
        test_1d_condition.cpp \
        test_2d_condition_2.cpp \
        test_2d_range.cpp \
        test_false_3d.cpp \
		test_non_one_coeff.cpp \
		test_cross_function.cpp \
		test_pragma.cpp


#test_3d.cpp

unit_test: run
run:$(patsubst %,diff-%,${cases}) $(patsubst %,diff_exe-%,${exe_cases}) 
	cat $(patsubst %,run_%/err.log,${cases}) $(patsubst %,%.exe.err.log,${exe_cases}) >err.log
run-%:  
	$(eval $@_src_file := $(patsubst run-%,%,$@))
	rm -rf run_${$@_src_file}; mkdir run_${$@_src_file};
	-cd run_${$@_src_file};touch err.log;  \
	  mars_opt ../test_case/${$@_src_file} -e c -p line_buffer -a impl_tool=sdaccel 2>tmp.log \
	  || echo "${$@_src_file} sdx compile failed" >> err.log; \
	cp rose_${$@_src_file} ${$@_src_file}.sdx.out; \
	mars_opt ../test_case/${$@_src_file} -e c -p line_buffer -a impl_tool=aocl 2>tmp.log \
	||  echo "${$@_src_file} aocl compile failed" >> err.log; \
	cp rose_${$@_src_file} ${$@_src_file}.aocl.out

diff-%:run-% 
	$(eval $@_src_file := $(patsubst diff-%,%,$@))
	-gcc -include test_case/header.h -fsyntax-only run_${$@_src_file}/rose_${$@_src_file} -I ${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin \
	  || echo ${$@_src_file} output syntax failed >> run_${$@_src_file}/err.log 
	-diff --brief run_${$@_src_file}/${$@_src_file}.sdx.out test_rule/${$@_src_file}.sdx.ref &>> run_${$@_src_file}/err.log
	-diff --brief run_${$@_src_file}/${$@_src_file}.aocl.out test_rule/${$@_src_file}.aocl.ref &>> run_${$@_src_file}/err.log
	-diff --brief run_${$@_src_file}/critical_message.rpt test_rule/${$@_src_file}.rpt.ref &>> run_${$@_src_file}/err.log

update:$(patsubst %,update-%,${cases})  $(patsubst %,update_exe-%,${exe_cases})
#update-%: run-%
update-%:
	$(eval $@_src_file := $(patsubst update-%,%,$@))
	-cp run_${$@_src_file}/${$@_src_file}.sdx.out test_rule/${$@_src_file}.sdx.ref 
	-cp run_${$@_src_file}/${$@_src_file}.aocl.out test_rule/${$@_src_file}.aocl.ref 
	-cp run_${$@_src_file}/critical_message.rpt test_rule/${$@_src_file}.rpt.ref

run_exe-%:
	$(eval $@_src_file := $(patsubst run_exe-%,%,$@))
	-cd test_sim; rm -rf ${$@_src_file}.exe; g++ -O3 ${$@_src_file}.cpp -o ${$@_src_file}.exe; \
	./${$@_src_file}.exe &> ${$@_src_file}.res; \
	mars_opt ${$@_src_file}.cpp -e c -p line_buffer; \
	rm -rf ${$@_src_file}.exe; \
	g++ -O3 rose_${$@_src_file}.cpp -o ${$@_src_file}.exe; \
	./${$@_src_file}.exe &> rose_${$@_src_file}.res
diff_exe-%: run_exe-%
	$(eval $@_src_file := $(patsubst diff_exe-%,%,$@))
	-diff --brief test_sim/rose_${$@_src_file}.cpp test_rule/rose_${$@_src_file}.out.ref &> ${$@_src_file}.exe.err.log
	-diff --brief test_sim/${$@_src_file}.res test_sim/rose_${$@_src_file}.res &>> ${$@_src_file}.exe.err.log
#update_exe-%:run_exe-%
update_exe-%:
	$(eval $@_src_file := $(patsubst update_exe-%,%,$@))
	- cp test_sim/rose_${$@_src_file}.cpp test_rule/rose_${$@_src_file}.out.ref

clean:
	rm -rf run_* test_sim/rose* test_sim/*.res test_sim/*.exe *.log *.json
.PHONY: clean run update diff 
