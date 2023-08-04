#cases=test.cpp test_local_channel.cpp
cases=test.cpp test2.cpp test3.cpp test4.cpp
unit_test: diff
diff: $(patsubst %,diff-%,$(cases))
	cat $(patsubst %,%.err.log,$(cases)) > err.log
run-%:%
	mars_opt $^  -e c -p structural_func_inline -a pure_kernel=on 2>$^.tmp.log || (cat $^.tmp.log > $^.err.log)

gen_ref: update

update:$(patsubst %,update-%,$(cases))
update-%:run-%
	$(eval $@_case_ := $(patsubst update-%,%,$@))
	-cp rose_${$@_case_} test_rule/${$@_case_}.out.ref

diff-%: run-%
	$(eval $@_case_ := $(patsubst diff-%,%,$@))
	-diff --brief rose_${$@_case_} test_rule/${$@_case_}.out.ref &>>${$@_case_}.err.log

clean:
	-rm -rf *.log *.o rose* *.json *.h *.ti
	-rm critical_message* conflict_type_def
