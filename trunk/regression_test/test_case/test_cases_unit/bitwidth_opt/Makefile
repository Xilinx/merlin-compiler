test_cases_list= test.c \
test1.cpp \
test2.cpp \
test3.cpp \
test4.cpp \
test5.cpp \
test6.cpp \
test8.cpp \
bs_kernel.cpp \
denoise_0_kernel.cpp \
denoise_1_kernel.cpp \
vec_add_kernel.cpp \
matmul_kernel.cpp \
matmul_2.cpp \
namd_kernel.cpp \
logistic_regression_kernel.cpp \
stencil.cpp \
test_single_assign.cpp \
test_small_unaligned_loop.cpp \
test_fine_grained_coalescing.cpp \
test_complex_call_path.cpp \
test_fine_grained_coalescing_invalid.cpp \
struct_copy.cpp  \
test_exact_range.cpp
test_cases_list_with_aggressive_opt= test_assert.cpp \
test_pragma.cpp  \
twonn_kernel.cpp

bw_list=16 32 64 128 256 512 

xilinx_only_list=test_ap_int.cpp test_ap_int_power_of_2.cpp test_write_only_unaligned_in_data_flow.cpp
intel_only_list=test_two_dimension_memcpy.c 
unit_test: run 

run: $(patsubst %,diff-%,${test_cases_list}) \
	$(patsubst %,diff-aggr-%,${test_cases_list_with_aggressive_opt}) \
	$(patsubst %,diff-bw-%,${bw_list}) c_sim run_invalid $(patsubst %,diff-xilinx-%,${xilinx_only_list}) \
    $(patsubst %,diff-intel-%,${intel_only_list})
	cat $(patsubst %,%.err.log,${test_cases_list}) \
	$(patsubst %,%.err.log,${test_cases_list_with_aggressive_opt}) \
	$(patsubst %,%.err.log,${bw_list}) c_sim/err.log invalid/err.log \
	$(patsubst %,%.err.log,${xilinx_only_list}) \
	$(patsubst %,%.err.log,${intel_only_list}) > err.log
c_sim:
	make test -C c_sim; 
run_invalid:
	make test -C invalid;
diff-%:out-%
	$(eval $@_src_file := $(patsubst diff-%,%,$@))
	  -file_name=`basename ${$@_src_file}`; \
	  file_name_wo_suffix=`basename $$file_name .cpp`; \
	  file_name_wo_suffix=`basename $$file_name_wo_suffix .c`; \
	  diff -du $${file_name_wo_suffix}.sdx.out test_rule/$${file_name_wo_suffix}.sdx.out.ref &>>${$@_src_file}.err.log; \
	  diff -du $${file_name_wo_suffix}.aocl.out test_rule/$${file_name_wo_suffix}.aocl.out.ref &>>${$@_src_file}.err.log;

diff-xilinx-%:out-xilinx-%
	$(eval $@_src_file := $(patsubst diff-xilinx-%,%,$@))
	  -file_name=`basename ${$@_src_file}`; \
	  file_name_wo_suffix=`basename $$file_name .cpp`; \
	  file_name_wo_suffix=`basename $$file_name_wo_suffix .c`; \
	  diff -du $${file_name_wo_suffix}.sdx.out test_rule/$${file_name_wo_suffix}.sdx.out.ref &>>${$@_src_file}.err.log

diff-intel-%:out-intel-%
	$(eval $@_src_file := $(patsubst diff-%,%,$@))
	  -file_name=`basename ${$@_src_file}`; \
	  file_name_wo_suffix=`basename $$file_name .cpp`; \
	  file_name_wo_suffix=`basename $$file_name_wo_suffix .c`; \
	  diff -du $${file_name_wo_suffix}.aocl.out test_rule/$${file_name_wo_suffix}.aocl.out.ref &>${$@_src_file}.err.log

diff-aggr-%: out-aggr-%
	$(eval $@_src_file := $(patsubst diff-aggr-%,%,$@))
	  -file_name=`basename ${$@_src_file}`; \
	  file_name_wo_suffix=`basename $$file_name .cpp`; \
	  file_name_wo_suffix=`basename $$file_name_wo_suffix .c`; \
	  diff -du $${file_name_wo_suffix}.sdx.out test_rule/$${file_name_wo_suffix}.sdx.out.ref &>>${$@_src_file}.err.log; \
	  diff -du $${file_name_wo_suffix}.aocl.out test_rule/$${file_name_wo_suffix}.aocl.out.ref &>>${$@_src_file}.err.log;
diff-bw-%: out-bw-%
	$(eval $@_bw := $(patsubst diff-bw-%,%,$@))
	 -diff -du test7_${$@_bw}.cpp.sdx.out test_rule/test7_${$@_bw}.cpp.sdx.out.ref &>>${$@_bw}.err.log; \
	 diff -du test7_${$@_bw}.c.sdx.out test_rule/test7_${$@_bw}.c.sdx.out.ref &>>${$@_bw}.err.log; \
	 diff -du test7_${$@_bw}.cpp.aocl.out test_rule/test7_${$@_bw}.cpp.aocl.out.ref &>>${$@_bw}.err.log; \
	 diff -du test7_${$@_bw}.c.aocl.out test_rule/test7_${$@_bw}.c.aocl.out.ref &>>${$@_bw}.err.log
out-%: %
	  -echo $<; rm -rf $<.err.log; touch $<.err.log; \
	  file_name=`basename $<`; \
	  file_name_wo_suffix=`basename $$file_name .cpp`; \
	  file_name_wo_suffix=`basename $$file_name_wo_suffix .c`; \
	  mars_opt -I ./ $< -e c -p bitwidth_opt -a impl_tool=sdaccel 2>$<.tmp.log || cat $<.tmp.log >> $<.err.log; \
	  gcc   -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing -I ${MERLIN_COMPILER_HOME}/source-opt/include/apint_include -fsyntax-only -c rose_$${file_name} 2>$<.tmp.log || (cat $<.tmp.log >> $<.err.log); \
	  mv rose_$${file_name} $${file_name_wo_suffix}.sdx.out; \
	  mars_opt -I ./ $< -e c -p bitwidth_opt -a impl_tool=aocl -a auto_register_size_threshold=1024 2>$<.tmp.log || cat $<.tmp.log >> $<.err.log; \
	  mv rose_$${file_name} $${file_name_wo_suffix}.aocl.out
out-xilinx-%: %
	  -echo $<; rm -rf $<.err.log; touch $<.err.log; \
	  file_name=`basename $<`; \
	  file_name_wo_suffix=`basename $$file_name .cpp`; \
	  file_name_wo_suffix=`basename $$file_name_wo_suffix .c`; \
	  mars_opt -I ./ $< -e c -p bitwidth_opt -a impl_tool=sdaccel 2>$<.tmp.log || cat $<.tmp.log >> $<.err.log; \
	  gcc   -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing -I ${MERLIN_COMPILER_HOME}/source-opt/include/apint_include -fsyntax-only -c rose_$${file_name} 2>$<.tmp.log || (cat $<.tmp.log >> $<.err.log); \
	  mv rose_$${file_name} $${file_name_wo_suffix}.sdx.out

out-intel-%: %
	  -echo $<; rm -rf $<.err.log; touch $<.err.log; \
	  file_name=`basename $<`; \
	  file_name_wo_suffix=`basename $$file_name .cpp`; \
	  file_name_wo_suffix=`basename $$file_name_wo_suffix .c`; \
	  mars_opt -I ./ $< -e c -p bitwidth_opt -a impl_tool=aocl 2>$<.tmp.log || cat $<.tmp.log >> $<.err.log; \
	  gcc  -fsyntax-only -c rose_$${file_name} 2>$<.tmp.log || (cat $<.tmp.log >> $<.err.log); \
	  mv rose_$${file_name} $${file_name_wo_suffix}.aocl.out


out-aggr-%: %
	  -echo $<; rm -rf $<.err.log; touch $<.err.log; \
	  file_name=`basename $<`; \
	  file_name_wo_suffix=`basename $$file_name .cpp`; \
	  file_name_wo_suffix=`basename $$file_name_wo_suffix .c`; \
	  mars_opt -I ./ $< -e c -p bitwidth_opt -a impl_tool=sdaccel -a aggressive_opt_for_cpp 2>$<.tmp.log || cat $<.tmp.log >> $<.err.log; \
	  gcc  -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing -I ${MERLIN_COMPILER_HOME}/source-opt/include/apint_include -fsyntax-only -c rose_$${file_name} 2>$<.tmp.log || (cat $<.tmp.log >> $<.err.log); \
	  mv rose_$${file_name} $${file_name_wo_suffix}.sdx.out; \
	  mars_opt -I ./ $< -e c -p bitwidth_opt -a impl_tool=aocl 2>$<.tmp.log || cat $<.tmp.log >> $<.err.log; \
	  mv rose_$${file_name} $${file_name_wo_suffix}.aocl.out
out-bw-%:
	$(eval $@_bw_ := $(patsubst out-bw-%,%,$@))
	rm -rf ${$@_bw}.err.log; touch ${$@_bw}.err.log
	-mars_opt test7_${$@_bw_}.cpp -a bus_bitwidth=${$@_bw_} -e c -p bitwidth_opt -a impl_tool=sdaccel 2>${$@_bw}.tmp.log || (cat ${$@_bw}.tmp.log >> ${$@_bw}.err.log);\
	 gcc  -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing -I ${MERLIN_COMPILER_HOME}/source-opt/include/apint_include -fsyntax-only -c rose_test7_${$@_bw_}.cpp 2>${$@_bw}.tmp.log || (cat ${$@_bw}.tmp.log >> ${$@_bw}.err.log); \
	 mv rose_test7_${$@_bw_}.cpp test7_${$@_bw_}.cpp.sdx.out; \
     mars_opt test7_${$@_bw_}.c -a bus_bitwidth=${$@_bw_} -e c -p bitwidth_opt -a impl_tool=sdaccel 2>${$@_bw}.tmp.log || (cat ${$@_bw}.tmp.log >> ${$@_bw}.err.log);\
	 gcc  -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing -I ${MERLIN_COMPILER_HOME}/source-opt/include/apint_include -fsyntax-only -c rose_test7_${$@_bw_}.c 2>${$@_bw}.tmp.log || (cat ${$@_bw}.tmp.log >> ${$@_bw}.err.log); \
	 mv rose_test7_${$@_bw_}.c test7_${$@_bw_}.c.sdx.out; \
     mars_opt test7_${$@_bw_}.cpp -a bus_bitwidth=${$@_bw_} -e c -p bitwidth_opt -a impl_tool=aocl 2>${$@_bw}.tmp.log || (cat ${$@_bw}.tmp.log >> ${$@_bw}.err.log);\
	 gcc  -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing -I ${MERLIN_COMPILER_HOME}/source-opt/include/apint_include -fsyntax-only -c rose_test7_${$@_bw_}.cpp 2>${$@_bw}.tmp.log || (cat ${$@_bw}.tmp.log >> ${$@_bw}.err.log); \
	 mv rose_test7_${$@_bw_}.cpp test7_${$@_bw_}.cpp.aocl.out; \
     mars_opt test7_${$@_bw_}.c -a bus_bitwidth=${$@_bw_} -e c -p bitwidth_opt -a impl_tool=aocl 2>${$@_bw}.tmp.log || (cat ${$@_bw}.tmp.log >> ${$@_bw}.err.log);\
	 gcc  -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing -I ${MERLIN_COMPILER_HOME}/source-opt/include/apint_include -fsyntax-only -c rose_test7_${$@_bw_}.c 2>${$@_bw}.tmp.log || (cat ${$@_bw}.tmp.log >> ${$@_bw}.err.log); \
	 mv rose_test7_${$@_bw_}.c test7_${$@_bw_}.c.aocl.out; 

clean_ref:
	rm -rf *.c *.cpp test_rule/*

gen_ref: update_ref
update_ref: 
	-cp -f invalid/critical_message.rpt test_rule/critical_message.ref 
	-for f in ${test_cases_list} ${test_cases_list_with_aggressive_opt} ${xilinx_only_list} ${intel_only_list}; do \
	  file_name=`basename $$f`; \
	  file_name_wo_suffix=`basename $$file_name .cpp`; \
	  file_name_wo_suffix=`basename $$file_name_wo_suffix .c`; \
	  cp -f $${file_name_wo_suffix}.sdx.out test_rule/$${file_name_wo_suffix}.sdx.out.ref ; \
	  cp -f $${file_name_wo_suffix}.aocl.out test_rule/$${file_name_wo_suffix}.aocl.out.ref ; \
	  done
	-for bw in 16 32 64 128 256 512; do \
	 cp -f test7_$${bw}.cpp.sdx.out test_rule/test7_$${bw}.cpp.sdx.out.ref ; \
	 cp -f test7_$${bw}.c.sdx.out test_rule/test7_$${bw}.c.sdx.out.ref ; \
	 cp -f test7_$${bw}.cpp.aocl.out test_rule/test7_$${bw}.cpp.aocl.out.ref ; \
	 cp -f test7_$${bw}.c.aocl.out test_rule/test7_$${bw}.c.aocl.out.ref ; \
	 done

clean:
	rm -rf *.o *.log *.c *.cpp
.PHONY: clean run update diff c_sim run_invalid 
