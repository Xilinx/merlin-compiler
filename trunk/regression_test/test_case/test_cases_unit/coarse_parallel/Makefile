cases = test_no_func.cpp test_func.cpp test_with_zero_indice.cpp kmeans_kernel.cpp test_func_with_constant_global.cpp \
		test_func_with_non_constant_global.cpp test_fake_dead.cpp test_func_with_assert.cpp test_func_with_macro.cpp \
		test_func_without_iterator.cpp test_func_with_dead_port.cpp test_non_canonical.cpp test_data_dep.cpp test_non_const_bound.cpp test_return.cpp \
		test_goto_stmt.cpp test_continue.cpp test_label.cpp
intel_cases = test_func_with_pragma.cpp test_stream_port.cpp \
		test_copy_in_out.cpp
xilinx_cases = test_hls_stream.cpp test_duplicate_only.cpp
.dummy:
	for f in ${cases}; \
	  do echo $$f ;\
	  done
unit_test: $(patsubst %,diff-intel-%,$(intel_cases)) $(patsubst %,diff-intel-%,$(cases)) $(patsubst %,diff-xilinx-%,$(cases) $(xilinx_cases))
	cat $(patsubst %.cpp,%.intel.err.log,$(intel_cases)) $(patsubst %.cpp,%.intel.err.log,$(cases)) $(patsubst %.cpp,%.xilinx.err.log,$(cases) $(xilinx_cases)) > err.log
run-intel-%:%
	$(eval $@case := $(patsubst %.cpp,%,$^))
	rm -rf ${$@case}.intel.err.log
	-rm -rf run_intel_${$@case}; mkdir run_intel_${$@case}; cd run_intel_${$@case}; mars_opt ../$^ -e c -p coarse_parallel -a impl_tool=aocl || echo ${$@case} intel fails >>../${$@case}.intel.err.log

diff-intel-%: run-intel-%
	$(eval $@case := $(patsubst diff-intel-%.cpp,%,$@))
	-diff --brief run_intel_${$@case}/rose_${$@case}.cpp test_rule/${$@case}.aocl.ref &>> ${$@case}.intel.err.log
run-xilinx-%: %
	$(eval $@case := $(patsubst %.cpp,%,$^))
	rm -rf ${$@case}.xilinx.err.log
	-rm -rf run_xilinx_${$@case}; mkdir run_xilinx_${$@case}; cd run_xilinx_${$@case}; mars_opt ../$^ -e c -p coarse_parallel -a impl_tool=sdaccel || echo ${$@case} xilinx fails >>../${$@case}.xilinx.err.log
diff-xilinx-%: run-xilinx-%
	$(eval $@case := $(patsubst diff-xilinx-%.cpp,%,$@))
	-diff --brief run_xilinx_${$@case}/rose_${$@case}.cpp test_rule/${$@case}.sdaccel.ref &>>${$@case}.xilinx.err.log
	
	
update:
	for f in ${cases}; \
	  do \
	  bf=`basename $$f .cpp`; \
	  cp run_xilinx_$${bf}/rose_$${bf}.cpp test_rule/$${bf}.sdaccel.ref; \
	  cp run_intel_$${bf}/rose_$${bf}.cpp test_rule/$${bf}.aocl.ref; \
	  done
	for f in ${intel_cases}; \
	  do \
	  bf=`basename $$f .cpp`; \
	  cp run_intel_$${bf}/rose_$${bf}.cpp test_rule/$${bf}.aocl.ref; \
	  done
	for f in ${xilinx_cases}; \
	  do \
	  bf=`basename $$f .cpp`; \
	  cp run_xilinx_$${bf}/rose_$${bf}.cpp test_rule/$${bf}.sdaccel.ref; \
	  done
