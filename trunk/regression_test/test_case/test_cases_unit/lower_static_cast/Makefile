all_test:   build unit_test
gen_ref: update_ref
unit_test: diff

run:
	-rm -rf err.log
	-touch err.log
	-rm -rf rose_*
	-rm -rf *.json
	-rm -rf critical_message.rpt
	-mars_opt test1.cpp -e c -p lower_cpp >test1.cpp.log 2>&1 \
	  && (mv rose_test1.cpp test1.cpp.out) \
	  || (mv test1.cpp.log test.cpp.out); 
	-rm -f test1.cpp.log rose*

update_ref:
	-cp -f test1.cpp.out test1.cpp.out.ref; \

diff:run 
	-diff --brief  test1.cpp.out test1.cpp.out.ref &>>err.log; \

clean:
	rm -rf rose* *.json *.rpt *.log
