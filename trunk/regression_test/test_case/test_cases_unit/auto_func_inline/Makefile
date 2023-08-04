
unit_test: diff

all_test: build unit_test
gen_ref: update_ref

run: clean
	-rm -rf err.log
	-touch err.log
	mars_opt test_function_inline_wo_def.c    -e c -p auto_func_inline -a auto_func_inline=aggressive 2>tmp.log || (cat tmp.log >> err.log)
	mars_opt test_function_inline_off.c    -e c -p auto_func_inline -a auto_func_inline=off 2>tmp.log || (cat tmp.log >> err.log)
	mars_opt test_function_inline.c    -e c -p auto_func_inline -a auto_func_inline=aggressive 2>tmp.log || (cat tmp.log >> err.log)
	mars_opt test_function_inline.c    -e c -p auto_func_inline -a auto_func_inline=aggressive > rose_test.log 
	mars_opt test_function_inline_normal.c -e c -p auto_func_inline 2>tmp.log || (cat tmp.log >> err.log)
	mars_opt test_function_inline_with_dataflow.cpp  -e c -p auto_func_inline -a auto_func_inline=aggressive -I ${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin 2>>tmp.log || (cat tmp.log >> err.log)
	mars_opt test_function_inline.cpp  -e c -p auto_func_inline -a auto_func_inline=aggressive 2>>tmp.log || (cat tmp.log >> err.log)
	mars_opt test_function_inline.cpp  -e c -p auto_func_inline -a auto_func_inline=aggressive >> rose_test.log 
	cat rose_test_function_inline.cpp >> rose_test_function_inline.c  || (echo FAIL >> err.log)

update_ref: run 
	-cp -f rose_test_function_inline.c  reference/
	-cp -f rose_test.log reference/
	-cp -f rose_test_function_inline_normal.c reference/
	-cp -f rose_test_function_inline_with_dataflow.cpp reference/

diff: run 
	-diff --brief  rose_test_function_inline.c reference/rose_test_function_inline.c >> err.log 2>&1
	-diff --brief  rose_test_function_inline_normal.c reference/rose_test_function_inline_normal.c >> err.log 2>&1
	-diff --brief  rose_test.log reference/rose_test.log >> err.log 2>&1
	-diff --brief  rose_test_function_inline_with_dataflow.cpp reference/rose_test_function_inline_with_dataflow.cpp >> err.log 2>&1

clean:
	rm -rf rose* *.o tmp.log err.log flex*.log history_*.json .finish_read_files


build:
	rm -rf  $(MERLIN_COMPILER_HOME)/build/bin/mars_opt_org
	make -C $(MERLIN_COMPILER_HOME) 

gdb:
	gdb --args mars_opt_org test_function_inline.c  -e c -p auto_func_inline -a auto_func_inline=aggressive -DFOCUS

gdb_cpp:
	-/usr/bin/gdb --args mars_opt_org test_function_inline.cpp  -e c -p auto_func_inline -DFOCUS
