FLAGZ ?= -I $(MERLIN_COMPILER_HOME)/source-opt/include/apint_include

unit_test:
	source ${MERLIN_COMPILER_HOME}/set_env/env.sh; mcheck ap-uint.cpp -- $(FLAGZ) 2>&1 | grep -o '\(error\|warning\):.\+' > ap-uint.out || true;
	touch err.log
	diff -u ap-uint.expected ap-uint.out > err.log || true;
update:
	cp  ap-uint.out ap-uint.expected	
clean:
	rm -f ap-uint.out err.log
