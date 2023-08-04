
unit_test: diff

all_test: build unit_test
gen_ref: update_ref

run: clean
	-rm -rf err.log
	-touch err.log
	mars_opt test_ivdep.c  -e c   -p loop_parallel -a impl_tool=aocl -a test_ivdep=on 2>tmp.log || (cat tmp.log >> err.log)
	mars_opt test_ivdep.c  -e c   -p loop_parallel -a impl_tool=aocl -a test_ivdep=on | grep -v HIS | grep -v Warning  | grep -v WARNING> rose_test.log 
	mars_opt test_ivdep.cpp  -e c -p loop_parallel -a impl_tool=aocl -a test_ivdep=on 2>>tmp.log || (cat tmp.log >> err.log)
	mars_opt test_ivdep.cpp  -e c -p loop_parallel -a impl_tool=aocl -a test_ivdep=on | grep -v HIS | grep -v Warning | grep -v WARNING >> rose_test.log 
	cat rose_test_ivdep.cpp >> rose_test_ivdep.c  || (echo FAIL >> err.log)
	-gcc -c rose_test_ivdep.c -std=c99 >> rose_test.log 2>&1   #no warning/error
	-gcc -c rose_test_ivdep.cpp >> rose_test.log 2>&1   #no warning/error
	-rm -rf flex*.log

update_ref: run
	-cp -f rose_test_ivdep.c  reference/
	-cp -f rose_test.log reference/

diff: run
	-diff --brief  rose_test_ivdep.c reference/rose_test_ivdep.c >> err.log 2>&1
	-diff --brief  rose_test.log reference/rose_test.log >> err.log 2>&1

clean:
	rm -rf critical_message.rpt rose* *.o tmp.log err.log flex*.log history_*.json .finish_read_files


build:
	rm -rf  $(MERLIN_COMPILER_HOME)/build/bin/mars_opt_org
	make -C $(MERLIN_COMPILER_HOME) 

gdb:
	which gdb
	-gdb --args mars_opt_org test_ivdep.c  -e c -p loop_parallel -a impl_tool=aocl -a test_ivdep=on -DFOCUS


gdb_cpp:
	-gdb --args mars_opt_org test_ivdep.cpp  -e c -p loop_parallel -a impl_tool=aocl -a test_ivdep=on -DFOCUS
