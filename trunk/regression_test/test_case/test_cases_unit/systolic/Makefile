
SHELL := /bin/bash

test_cases := matmul

unit_test:
	@rm -rf err.log
	@for case in $(test_cases); do \
		make -C $$case clean &>> /dev/null; \
		make -C $$case unit_test &>> /dev/null; \
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
		make -C $$case clean &> /dev/null; \
	done
	@rm -rf *.log
