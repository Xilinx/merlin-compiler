
unit_test: diff

all_test: build unit_test
gen_ref: update_ref

run:
	-rm -rf err.log
	-touch err.log
	mars_opt test_function_inline.c  -e c -p function_inline_by_pragma 2>tmp.log || (cat tmp.log >> err.log)

update_ref:run 
	-cp -f rose_test_function_inline.c  reference/

diff:run 
	-diff --brief  rose_test_function_inline.c reference/rose_test_function_inline.c >> err.log 2>&1

clean:
	rm -rf rose* *.o tmp.log err.log flex*.log history_*.json


build:
	source ~/fcs_setting64.sh; cd ~/WORK2; make all; cd -

gdb:
	-/usr/bin/gdb --args mars_opt_org test_function_inline.c  -e c -p function_inline_by_pragma
