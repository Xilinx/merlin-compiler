
SHELL := /bin/bash

# test_cases := test1 test2 test3 test4 test5 test6 
# test_cases := test1
test_cases := test1 test2 test3 test4 test5 test6 test10 \
	test12 test15 test16 test17 test18 test19

generate:
	@for case in $(test_cases); do \
		make -C $$case/src clean &>> /dev/null; \
		make -C $$case/src generate_golden_extract; \
	done

copy_makefile:
	@for case in $(test_cases); do \
		cp test1/src/Makefile $$case/src; \
	done

extract_unit_test:
	@rm -rf err.log
	@for case in $(test_cases); do \
		make -C $$case/src clean &>> /dev/null; \
		make -C $$case/src extract_unit_test; \
		if [ ! -f "$$case/src/err.log" ]; then \
		echo "$$case/src/err.log doesn't exist" >> err.log; \
		fi; \
		if [ `stat -c %s $$case/src/err.log` -eq 0 ]; then \
			echo "$$case...OK" >> err.log; \
			echo "$$case...OK"; \
		else \
			cat $$case/src/err.log >> err.log; \
			break; \
		fi; \
	done
	@touch err.log

clean:
	@for case in $(test_cases); do \
		echo "cleaning $$case..."; \
		make -C $$case/src clean &> /dev/null; \
	done
	@rm -rf *.log
