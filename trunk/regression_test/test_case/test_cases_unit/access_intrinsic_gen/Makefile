all_test:   build unit_test
gen_ref: update_ref
unit_test: diff
supported_cases = test_basic.c test_multi_same_dim.c test_multi_diff_dim.c test_multi_unknown_dim.c 

run:
	-rm -rf err.log
	-touch err.log
	-rm -rf rose_*
	-rm -rf critical_message.rpt
	-for f in ${supported_cases}; \
	  do \
	  mars_opt $$f -e c -p gen_access_range 2>tmp.log || (cat tmp.log >> err.log); \
	  done

update_ref: run
	-for f in ${supported_cases}; \
	  do \
	    cp -f rose_$$f test_rule/rose_$$f; \
	  done

diff: run
	-for f in ${supported_cases}; \
	  do \
	   diff --brief  rose_$$f test_rule/rose_$$f &>>err.log; \
	  done

clean:
	rm -rf rose* *.o
