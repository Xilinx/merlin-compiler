PYENV="${MERLIN_COMPILER_HOME}/build/venv"

run:
	@source ${PYENV}/bin/activate; \
		rm -rf temp; cp -rf work temp; mkdir temp/profile; \
		export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/scripts:$$PYTHONPATH; \
		export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/libs/opentuner:$$PYTHONPATH; \
		export MERLIN_DSE_MOCK="mock@`pwd`/temp/mock_db.pkl"; \
			cd temp; \
			mv mock/mock_db.pkl .; \
			python ../run_test.py run; \
			cd ..; \
		unset MERLIN_DSE_MOCK; \
	deactivate

update:
	@source ${PYENV}/bin/activate; \
		rm -rf temp; cp -rf work temp; mkdir temp/profile; \
		export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/scripts:$$PYTHONPATH; \
		export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/libs/opentuner:$$PYTHONPATH; \
		export MERLIN_DSE_MOCK="save@`pwd`/temp/"; \
			cd temp; \
			python ../run_test.py run; \
			cd ..; \
		unset MERLIN_DSE_MOCK; \
	deactivate
	@rm -rf refs; mkdir refs;
	@cp temp/ds_*.json refs/
	@rm -rf mock work/mock; mkdir mock
	@mv temp/mock_db.pkl mock/
	@mv mock work/

diff: run
	@source ${PYENV}/bin/activate; \
		export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/scripts:$$PYTHONPATH; \
		export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/libs/opentuner:$$PYTHONPATH; \
			python ./run_test.py check 2>> err.log; \
	deactivate

clean:
	@rm -rf temp *.log *.xml .merlin*
