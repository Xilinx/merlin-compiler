
SHELL := /bin/bash

# test_cases := test1 test2 test3 test4 test5 test6 test12 test15 test16 test17 test18 test19
test_cases := test1 test2

generate:
	@for case in $(test_cases); do \
		make -C $$case distclean &>> /dev/null; \
		make -C $$case merlincc; \
	done

unit_test:
	@rm -rf err.log
	@for case in $(test_cases); do \
		make -C $$case distclean &>> /dev/null; \
		make -C $$case run; \
		if [ ! -f "$$case/err.log" ]; then \
		echo "$$case/err.log doesn't exist" >> err.log; \
		fi; \
		if [ `stat -c %s $$case/err.log` -eq 0 ]; then \
			echo "$$case...OK"; \
		else \
			cat $$case/err.log >> err.log; \
			break; \
		fi; \
	done
	@touch err.log

clean:
	@for case in $(test_cases); do \
		echo "cleaning $$case..."; \
		make -C $$case distclean &> /dev/null; \
	done
	@rm -rf *.log
