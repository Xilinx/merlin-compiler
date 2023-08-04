PYENV="${MERLIN_COMPILER_HOME}/build/venv"
CMD="${MERLIN_COMPILER_HOME}/optimizers/autodse/scripts/ds_proc_builder.pyc"
RULE_CASES=$(wildcard src/rule_cases/*)

define run_test
	@mode=$(1); \
	echo "Test building design space with mode $$mode"; \
	rm -rf $$mode.temp; cp -rf src/$$mode $$mode.temp; \
	source ${PYENV}/bin/activate; \
		export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/libs/opentuner:$$PYTHONPATH; \
		export MERLIN_CURR_PROJECT=`pwd`; \
		cd $$mode.temp; \
		python ${CMD} --no-profiling --type=$$mode .; \
		deactivate; \
		cd ../
endef

define run_rules
	@echo "Test building design space with errors"; \
	rm -rf rules.temp; cp -rf src/custom rules.temp; \
	source ${PYENV}/bin/activate; \
		export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/libs/opentuner:$$PYTHONPATH; \
		export MERLIN_CURR_PROJECT=`pwd`; \
		cd rules.temp; \
		for case in ${RULE_CASES}; do \
			cp ../$$case __merlinkernel_vec_add_kernel.cpp; \
			echo "== Testing $$case" >> out.log; \
			python ${CMD} --no-profiling --type=custom . 2>> out.log; \
		done; \
	grep ERROR out.log > msg.log; \
	exit 0
endef

define run_update
	@mode=$(1); \
	rm -rf refs/$$mode; mkdir -p refs/$$mode; \
	cp -f $$mode.temp/__merlinkernel* refs/$$mode/; \
	cp -f $$mode.temp/auto_pragmas.json refs/$$mode/; \
	cp -f $$mode.temp/ds*.json refs/$$mode/
endef

define run_diff
	@mode=$(1); \
	diff --brief $$mode.temp/__merlinkernel_vec_add_kernel.cpp refs/$$mode/__merlinkernel_vec_add_kernel.cpp &>> err.log; \
	python ../common/check_json.py $$mode.temp/auto_pragmas.json refs/$$mode/auto_pragmas.json &>> err.log; \
	for file in refs/$$mode/ds_*.json; do \
		python "$$file" "$$mode.temp/$${file##*/}" &>> err.log; \
	done
endef

define run_diff_msgs
	@file1=$(1); \
	file2=$(2); \
	while read line; do \
		grep -Fxq "$$line" $$file1; \
	    if [ $$? -ne 0 ]; then \
	        echo "\"$$line\" is missing in $$file1" >> err.log; \
	    fi; \
	done < $$file2
endef

run:
	$(call run_test,full)
	$(call run_test,custom)
	$(call run_rules)

update: run
	$(call run_update,full)
	$(call run_update,custom)
	cp rules.temp/msg.log refs/rules.ref
	
diff: run
	$(call run_diff,full)
	$(call run_diff,custom)
	$(call run_diff_msgs,rules.temp/msg.log,refs/rules.ref)
	$(call run_diff_msgs,refs/rules.ref,rules.temp/msg.log)

clean:
	find . -name "rose*"  -prune -exec rm -rf {} \;
	find . -name "flex*"  -prune -exec rm -rf {} \;
	find . -name "*.log"  -prune -exec rm -rf {} \;
	find . -name "*.rpt"  -prune -exec rm -rf {} \;
	find . -name "*.temp" -prune -exec rm -rf {} \; 
	find . -name ".*idx*" -prune -exec rm -rf {} \;
