ifeq (,$(TARGET))
TARGET := test
endif

ifeq (,$(MAKE))
  $(error "need to define the custom make-rpt wrapper")
endif

CASES	  := $(shell cat cases.list | awk '{print $$1}')
ERR_FILES := $(patsubst %, %/err.log, $(CASES))
CHK_FILES := $(patsubst %, %/check,   $(CASES))

.SECONDARY:
	$(ERR_FILES)

all: $(CHK_FILES)

%/err.log: %
	@$(MAKE) $(TARGET) $< &>> $<.log

%/check: %/err.log %
	@if [ -f "$<" ] && [ $$(wc -l < $<) -eq 0 ] && [ ! -f $(word 2,$^).error ]; then \
		printf " - %-50s PASS\n" $(word 2,$^); \
	elif [ -f $(word 2,$^).timeout ]; then \
		printf " - %-50s TIMEOUT\n" $(word 2,$^); \
	else \
		printf " - %-50s FAIL\n" $(word 2,$^); \
		touch "$(word 2,$^).error"; \
	fi 

.PHONY: all
