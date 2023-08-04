
unit_test: init 
	make diff
init:
	-rm -rf err.log
	-touch err.log
	-rm -rf rose_*
run: run_typedef run_reference run_return run_global false_case

run_typedef:
	-mars_opt test_case/postwrap_typedef.cpp -e c -p postwrap_process -a test_typedef_spread || echo fails >> err.log
	-gcc -fsyntax-only rose_postwrap_typedef.cpp 2>tmp.log || cat tmp.log >> err.log
	-mars_opt test_case/typedef_in_extern_func.c -e c -p postwrap_process -a test_typedef_spread || echo fails >> err.log
	-gcc -fsyntax-only rose_typedef_in_extern_func.c 2>tmp.log || cat tmp.log >> err.log
	

run_reference:
	-mars_opt test_case/postwrap_reference.cpp -e c -p postwrap_process -a test_reference_replace
	-gcc -fsyntax-only rose_postwrap_reference.cpp 2>tmp.log || cat tmp.log >> err.log

run_return:
	-mars_opt test_case/postwrap_return.cpp -e c -p postwrap_process -a test_return_replace || echo fails >> err.log
	-gcc -fsyntax-only rose_postwrap_return.cpp 2>tmp.log || cat tmp.log >> err.log
	-mars_opt test_case/postwrap_return_hls.cpp -e c -p postwrap_process -a test_return_replace -a impl_tool=sdaccel -a tool_version=vivado_hls || echo fails >> err.log
	-gcc -fsyntax-only rose_postwrap_return_hls.cpp 2>tmp.log || cat tmp.log >> err.log

run_global:
	-mars_opt test_case/postwrap_global.cpp -e c -p postwrap_process -a test_global_convert || echo fails >> err.log
	-gcc -fsyntax-only rose_postwrap_global.cpp 2>tmp.log || cat tmp.log >> err.log
	-mars_opt test_case/extern-block-var.c -e c -p postwrap_process -a test_global_convert || echo fails >> err.log
	-gcc -fsyntax-only rose_extern-block-var.c 2>tmp.log || cat tmp.log >> err.log
	-mars_opt test_case/postwrap_global_hls.cpp -e c -p postwrap_process -a test_global_convert -a impl_tool=sdaccel -a tool_version=vivado_hls  || echo fails >> err.log
	-gcc -fsyntax-only rose_postwrap_global_hls.cpp 2>tmp.log || cat tmp.log >> err.log
	-mars_opt test_case/extern-block-var-hls.c -e c -p postwrap_process -a test_global_convert -a impl_tool=sdaccel -a tool_version=vivado_hls || echo fails >> err.log
	-gcc -fsyntax-only rose_extern-block-var-hls.c 2>tmp.log || cat tmp.log >> err.log

false_case:
	-rm -rf critical_message.rpt; 
	-mars_opt test_case/false_case_0.cpp -e c -p postwrap_process  
	-mars_opt test_case/false_case_1.cpp -e c -p postwrap_process  
	-mars_opt test_case/false_case_2.cpp -e c -p postwrap_process -a test_global_checker 
	-mars_opt test_case/false_case_3.cpp -e c -p postwrap_process -a test_global_checker 
	-mars_opt test_case/false_case_4.cpp -e c -p postwrap_process -a test_global_checker -a impl_tool=aocl 
	-mars_opt test_case/postwrap_non_initialized_const_global.cpp -e c -p postwrap_process -a test_global_checker
	-mars_opt test_case/postwrap_non_initialized_const_global.c -e c -p postwrap_process -a test_global_checker
	-mars_opt test_case/postwrap_external_global_array.c -e c -p postwrap_process -a test_global_checker
#	-grep "Kernel return expresion can not be aggregate initilizer" critical_message.rpt 2>tmp.log || (echo "Error msg not found">> err.log && cat tmp.log >> err.log)

diff: run
	-diff --brief rose_postwrap_typedef.cpp test_rule/postwrap_typedef.cpp.ref >> err.log
	-diff --brief rose_postwrap_reference.cpp test_rule/postwrap_reference.cpp.ref >> err.log
	-diff --brief rose_postwrap_return.cpp test_rule/postwrap_return.cpp.ref >> err.log
	-diff --brief rose_postwrap_global.cpp test_rule/postwrap_global.cpp.ref >> err.log
	-diff --brief rose_extern-block-var.c test_rule/extern-block-var.c.ref >> err.log
	-diff --brief rose_postwrap_return_hls.cpp test_rule/postwrap_return_hls.cpp.ref >> err.log
	-diff --brief rose_postwrap_global_hls.cpp test_rule/postwrap_global_hls.cpp.ref >> err.log
	-diff --brief rose_extern-block-var-hls.c test_rule/extern-block-var-hls.c.ref >> err.log
	-diff --brief rose_typedef_in_extern_func.c test_rule/typedef_in_extern_func.ref >> err.log
	-diff --brief critical_message.rpt test_rule/critical_message.ref >> err.log

update:
	cp rose_postwrap_typedef.cpp test_rule/postwrap_typedef.cpp.ref
	cp rose_postwrap_reference.cpp test_rule/postwrap_reference.cpp.ref
	cp rose_postwrap_return.cpp test_rule/postwrap_return.cpp.ref
	cp rose_postwrap_global.cpp test_rule/postwrap_global.cpp.ref
	cp rose_extern-block-var.c test_rule/extern-block-var.c.ref
	cp critical_message.rpt test_rule/critical_message.ref
	cp rose_typedef_in_extern_func.c test_rule/typedef_in_extern_func.ref
	cp rose_postwrap_return_hls.cpp test_rule/postwrap_return_hls.cpp.ref 
	cp rose_postwrap_global_hls.cpp test_rule/postwrap_global_hls.cpp.ref 
	cp rose_extern-block-var-hls.c test_rule/extern-block-var-hls.c.ref 


clean:
	-rm *.log  rose_* *.json *.o
