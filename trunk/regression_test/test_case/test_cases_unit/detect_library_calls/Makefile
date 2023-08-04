unit_test: diff
run: 
	rm -rf err.log
	touch err.log
	rm -rf critical_message.rpt rose* *.json; 
	-mars_opt test1.c -e c -p detect_library_calls -a impl_tool=sdaccel
	-mars_opt test2.c -e c -p detect_library_calls -a impl_tool=sdaccel
	-mars_opt test3.cpp -e c -p detect_library_calls -a impl_tool=sdaccel
  
clean:
	rm .*; rm *.log; rm rose*; rm *.rpt; rm *.json;

gen_ref: update

update:
	mv critical_message.rpt critical_message.ref

diff: run
	-diff -du critical_message.rpt critical_message.ref &>> err.log


