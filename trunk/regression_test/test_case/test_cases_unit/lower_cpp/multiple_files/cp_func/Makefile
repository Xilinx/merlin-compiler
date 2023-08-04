SRCS := cf_t1.cpp cf_t2.cpp

outs:
	rm -f err.log
	mars_opt $(SRCS) -e c -p lower_cpp > srcs.log 2>&1 || mv srcs.log err.log

run: outs
	$(foreach src,$(SRCS),diff -u rose_$(src) $(src).expected >> err.log || true;)
	touch critical_message.rpt
	cat critical_message.rpt >> err.log

update: outs
	$(foreach src,$(SRCS),cp rose_$(src) $(src).expected;)

# The mars_test test runner expects this target to exist. TODO: Remove this once
# mars_test has been updated.
unit_test: run
diff: run
gen_ref: update
update_ref: update

clean:
	rm -f critical_message.rpt srcs.log err.log *.cpp.out flex*.log rose_*.cpp \
		rose.log history_*.json

.PHONY: clean update outs diff
