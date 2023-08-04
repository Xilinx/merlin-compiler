unit_test: diff
run:
	source ${MERLIN_COMPILER_HOME}/set_env/env.sh; mcheck lambda.cpp c99.c 2>&1 | grep -o '\(error\|warning\):.\+' > diagnostics.out || true;
diff: run 
	touch err.log
	diff -u diagnostics.expected diagnostics.out > err.log || true;
update: run	
	cp diagnostics.out diagnostics.expected

clean:
	rm -f diagnostics.out err.log
