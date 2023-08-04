
cases=test.cpp test_range.cpp test_sub.cpp test_opt.cpp test_critical_loop.cpp test_xilinx_channel.cpp test_do_while.cpp \
			test_init_loop.cpp test_access_pattern.cpp test_struct.cpp test_nested_trivial_loop.cpp
unit_test: $(patsubst %,diff-%,${cases}) 
	cat $(patsubst %,%.err.log,${cases}) > err.log
run-%: %
	-rm -rf $^.err.log 
	-touch $^.err.log
	-mars_opt $^  -e c -p auto_parallel -a impl_tool=sdaccel -a pure_kernel=on 2>$^.tmp.log || (cat $^.tmp.log >> $^.err.log)

gen_ref: update

update:$(patsubst %,update-%,${cases})
update-%:run-%
	$(eval $@curr_case := $(patsubst update-%,%,$@))
	-cp rose_${$@curr_case} test_rule/rose_${$@curr_case}.ref

diff-%: run-%
	$(eval $@curr_case := $(patsubst diff-%,%,$@))
	-diff --brief rose_${$@curr_case} test_rule/rose_${$@curr_case}.ref &>>${$@curr_case}.err.log

clean:
	-rm -rf *.log *.o rose* *.json *.h *.ti
	-rm critical_message* conflict_type_def
