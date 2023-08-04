
SHELL := /bin/bash

test_cases := test1 test2 test3 test4 \
	test5 test6 test7 test8 \
	test9 test10 test11

unit_test:
	@rm -rf err.log
	@for case in $(test_cases); do \
		make -C $$case clean &>> /dev/null; \
		make -C $$case run &>> /dev/null; \
		if [ -f $$case/systolic_array_kernel1.cpp ]; then \
			echo "$$case...OK"; \
		else \
			cat "$$case...Not OK" >> err.log; \
			break; \
		fi; \
	done
	@touch err.log

clean:
	@for case in $(test_cases); do \
		echo "cleaning $$case..."; \
		make -C $$case clean &> /dev/null; \
	done
	@rm -rf *.log
