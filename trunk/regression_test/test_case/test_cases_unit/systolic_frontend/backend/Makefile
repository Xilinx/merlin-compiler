
SHELL := /bin/bash

test_cases := test1

unit_test:
	@rm -rf err.log
	@for case in $(test_cases); do \
		make -C $$case clean &>> /dev/null; \
		make -C $$case test &>> /dev/null; \
		make -C $$case flow &>> /dev/null; \
		if [ `stat -c %s $$case/step6/err.log` -eq 0 ]; then \
			echo "$$case...OK"; \
		else \
			cat $$case/step6/err.log >> err.log; \
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
