PYENV="${MERLIN_COMPILER_HOME}/build/venv"
SRC="`pwd`/temp/implement/opencl_gen"
CMD="${MERLIN_COMPILER_HOME}/optimizers/autodse/scripts/dse.pyc"

define test_impl
	@case=$(1); \
	max_part=$(2); \
	mocking=$(3); \
	source ${PYENV}/bin/activate; \
	echo "Checking DSE flow with $$case"; \
	rm -rf $$case.temp; mkdir $$case.temp; \
	rsync -av -progress prj_$$case/ $$case.temp/ 2>&1 >/dev/null; \
	cp $$case.temp/directive.xml .; \
	export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/scripts:$$PYTHONPATH; \
	export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/libs/opentuner:$$PYTHONPATH; \
	export MERLIN_CURR_PROJECT=`pwd`/$$case.temp/; \
	if [ $$mocking -ne 0 ]; then \
		export MERLIN_DSE_MOCK="mock@`pwd`/$$case.temp/mock_db.pkl"; \
	else \
		export MERLIN_DSE_MOCK="save@`pwd`/$$case.temp/"; \
		rm -f $$case.temp/mock_db.pkl; \
	fi; \
		cd $$case.temp/implement/dse; \
		python ${CMD} --working-dir=./tmp --max-partition=$$max_part ${SRC} --test-limit=5 2>&1;\
		export ret=$$?; \
		if [ $$ret -ne 0 ]; then \
			echo "Exit code for $$case is not 0: $$ret" >> ../../../err.log; \
		fi; \
 		grep ERROR auto_dse.log >> ../../../err.log; \
		cd ../../../; \
	deactivate; \
	unset MERLIN_DSE_MOCK; \
	echo "Check DSE flow with $$case done"
endef

run:
	@rm -f err.log
	$(call test_impl,aocl,1,1)
	$(call test_impl,sdaccel,32,1)
	$(call test_impl,cache,32,1)
	$(call test_impl,custom,32,1)
	@touch err.log

update:
	$(call test_impl,aocl,1,0)
	mv aocl.temp/mock_db.pkl prj_aocl/
	$(call test_impl,sdaccel,32,0)
	mv sdaccel.temp/mock_db.pkl prj_sdaccel/
	$(call test_impl,cache,32,0)
	mv cache.temp/mock_db.pkl prj_cache/
	$(call test_impl,custom,32,0)
	mv custom.temp/mock_db.pkl prj_custom/

diff: run

clean:
	@rm -rf *.temp exit_code *.log *.xml &> /dev/null
