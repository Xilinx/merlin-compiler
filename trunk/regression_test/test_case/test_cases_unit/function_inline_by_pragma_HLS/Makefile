case=test_function_inline.c
unit_test: diff

all_test: build unit_test
gen_ref: update_ref

run:
	-rm -rf err.log
	-touch err.log

update_ref:$(patsubst %,update-%,$(case))
update-%: run-%
	$(eval $@_case_ := $(patsubst update-%,%,$@))
	-cp rose_${$@_case_} reference/rose_${$@_case_} 

diff: $(patsubst %,diff-%,$(case))
	cat $(patsubst %,%.err.log,$(case)) > err.log
diff-%: run-%
	$(eval $@_case_ := $(patsubst diff-%,%,$@))
	-diff --brief  rose_${$@_case_} reference/rose_${$@_case_} >> ${$@_case_}err.log 2>&1
run-%: %
	rm -rf $^.err.log; touch $^.err.log
	-mars_opt $^ -e c -p function_inline_by_pragma_HLS  2>$^.tmp.log || cat $^.tmp.log >> $^.err.log


clean:
	rm -rf rose* *.o tmp.log err.log flex*.log history_*.json


gdb:
	-/usr/bin/gdb --args mars_opt_org test_function_inline.c  -e c -p function_inline_by_pragma
