
unit_test: diff

all_test: build unit_test
gen_ref: run update_ref

run:
	-rm -rf err.log
	-touch err.log
	mars_opt test_autoreg.c  -e c   -p loop_parallel -a impl_tool=aocl -a test_autoreg=on 2>tmp.log || (cat tmp.log >> err.log)
	mars_opt test_autoreg.c  -e c   -p loop_parallel -a impl_tool=aocl -a test_autoreg=on | grep -v HIS | grep -v Warning  | grep -v WARNING> rose_test.log 
	mars_opt test_autoreg.cpp  -e c -p loop_parallel -a impl_tool=aocl -a test_autoreg=on 2>>tmp.log || (cat tmp.log >> err.log)
	mars_opt test_autoreg.cpp  -e c -p loop_parallel -a impl_tool=aocl -a test_autoreg=on | grep -v HIS | grep -v Warning | grep -v WARNING >> rose_test.log 
	cat rose_test_autoreg.cpp >> rose_test_autoreg.c  || (echo FAIL >> err.log)
	-gcc -c rose_test_autoreg.c -std=c99 -Wno-attributes >> rose_test.log 2>&1   #no warning/error
	-gcc -c rose_test_autoreg.cpp        -Wno-attributes >> rose_test.log 2>&1   #no warning/error
	-rm -rf flex*.log

update_ref:
	-cp -f rose_test_autoreg.c  reference/
	-cp -f rose_test.log reference/

diff: run
	-diff --brief  rose_test_autoreg.c reference/rose_test_autoreg.c >> err.log 2>&1
	-diff --brief  rose_test.log reference/rose_test.log >> err.log 2>&1

clean:
	rm -rf critical_message.rpt rose* *.o tmp.log err.log flex*.log history_*.json .finish_read_files


build:
	rm -rf  $(MERLIN_COMPILER_HOME)/build/bin/mars_opt_org
	make -C $(MERLIN_COMPILER_HOME) 

gdb:
	which gdb
	-gdb --args mars_opt_org test_autoreg.c  -e c -p loop_parallel -a impl_tool=aocl -a test_autoreg=on -DFOCUS


gdb_cpp:
	-gdb --args mars_opt_org test_autoreg.cpp  -e c -p loop_parallel -a impl_tool=aocl -a test_autoreg=on -DFOCUS
