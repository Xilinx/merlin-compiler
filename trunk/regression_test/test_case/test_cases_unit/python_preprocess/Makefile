cases = include_stmt.cpp no_expansion.c
unit_test:
	rm -rf err.log
	touch err.log
	-for f in ${cases}; do \
	  cp $$f tmp_$$f; \
	  python ${MERLIN_COMPILER_HOME}/mars-gen/scripts/merlin_flow/preprocess_remove_directive.py  tmp_$$f; \
	  gcc -fsyntax-only tmp_$$f || echo "fail to preprocess $$f" >>err.log; done

