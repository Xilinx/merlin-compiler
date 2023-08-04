all_test:   build unit_test
gen_ref: update_ref
unit_test: diff
supported_cases = test_simple_pointer.c  \
				  test_pointer_self_assign.c \
				  test_array_with_multiple_defs.cpp

unsupported_cases =  test_pointer_switch.c 
run:
	-rm -rf err.log
	-touch err.log
	-rm -rf rose_*
	-rm -rf critical_message.rpt
	-for f in ${supported_cases}; \
	  do \
	  mars_opt $$f -e c -p detect_non_static_pointer -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log); \
	  gcc -c -std=c99 -fsyntax-only rose_$$f 2>tmp.log || (cat tmp.log >> err.log); \
	  done
	-for f in ${unsupported_cases}; \
	  do \
	  mars_opt $$f -e c -p detect_non_static_pointer -a impl_tool=sdaccel; \
	  done


#   -mars_opt test_local2.cpp -e c -p detect_non_static_pointer 2>tmp.log || (cat tmp.log >> err.log)
#	-mars_opt test_stl_member.cpp -e c -p detect_non_static_pointer 2>tmp.log || (cat tmp.log >> err.log)

update_ref: run
	-for f in ${supported_cases}; \
	  do \
	    cp -f rose_$$f test_rule/rose_$$f; \
	  done
	-cp -f  critical_message.rpt test_rule/critical_message.ref

diff: run 
	-for f in ${supported_cases}; \
	  do \
	   diff --brief  rose_$$f test_rule/rose_$$f &>>err.log; \
	  done
	-diff --brief  critical_message.rpt test_rule/critical_message.ref &>> err.log

clean:
	rm -rf rose* *.o
