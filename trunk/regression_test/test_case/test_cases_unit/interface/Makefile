
all_test:   build unit_test
gen_ref: update
unit_test: diff
run: clean
	-rm -rf err.log;
	-touch err.log;
	-cd variable_depth; make unit_test; cd -;
	-cat variable_depth/err.log >> err.log;

update:run 
	-cd variable_depth; make update; cd -;

diff:run 
	-cd variable_depth; make diff; cd -;

clean:
	-cd variable_depth; make clean; cd -;
