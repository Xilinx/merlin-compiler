.PHONY: test
.PHONY: update

unit_test:
	-rm err.log
	-touch err.log
	-make -C test_uncover_function test
	-cat test_uncover_function/err.log >> err.log
