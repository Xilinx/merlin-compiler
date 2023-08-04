FLAGZ ?= -I $(MERLIN_COMPILER_HOME)/source-opt/include/apint_include

unit_test: diff
run:
	source ${MERLIN_COMPILER_HOME}/set_env/env.sh; mcheck tilde-exp.cpp -- $(FLAGZ) 2>&1 | grep -o '\(error\|warning\):.\+' > tilde-exp.out || true;
diff: run
	touch err.log
	diff -u tilde-exp.expected tilde-exp.out > err.log || true;
update: run 	
	cp tilde-exp.out tilde-exp.expected 

clean:
	rm -f tilde-exp.out err.log
