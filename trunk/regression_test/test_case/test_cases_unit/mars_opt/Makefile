unit_test: check
init:
	rm -rf err.log
	touch err.log 
run: init
	-mars_opt -e c test.c -p invalid_pass &>invalid_pass.log 
	-ulimit -s 64; mars_opt -e c test.c -p none &>invalid_stack.log
	-mars_opt -e c test.c -d &>invalid_option.log
	-mars_opt -e cl test2.cl -p none 2>tmp.log || cat tmp.log &>> err.log
	-mars_opt -e c test.c -p none -a impl_tool=invalid_vendor &>invalid_vendor.log
	-mars_opt -e c test.c -p none -t top 2>tmp.log || cat tmp.log &>> err.log

check: run
	-grep "Unrecognized pass \"invalid_pass\"" invalid_pass.log || echo "fail to catch invalid pass" >> err.log
	-grep "setrlimit returned result" invalid_stack.log || echo "fail to catch invalid stack setting" >> err.log
	-grep "Usage: mars_opt" invalid_option.log || echo "fail to catch invalid option" >> err.log
