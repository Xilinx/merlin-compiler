CASES	  := $(shell cat ./cases.list)
ERR_FILES := $(patsubst %, %/err.log, $(CASES))
CHK_FILES := $(patsubst %, %/check,   $(CASES))
CLN_FILES := $(patsubst %, %/clean,   $(CASES))

all: $(CHK_FILES)

%/err.log: %
	@make unit_test -C $< &>> test.log

%/check: %/err.log %
	@if [ -z $(wc -l < $<)]; then \
		printf "%-30s PASS\n" $(word 2,$^); \
	else \
		printf "%-30s FAIL\n" $(word 2,$^); \
		exit 1; \
	fi 
	
%/clean: %
	make clean -C $<

clean: $(CLN_FILES)

.PHONY: all clean
