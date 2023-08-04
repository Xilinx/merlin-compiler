PYENV="${MERLIN_COMPILER_HOME}/build/venv"

run:
	@source ${PYENV}/bin/activate; \
		for impl in aocl sdaccel; do \
			rm -rf $$impl.temp; mkdir -p $$impl.temp; \
			export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/scripts:$$PYTHONPATH; \
			export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/libs/opentuner:$$PYTHONPATH; \
				echo "Checking $$impl"; \
				python ./run_test.py ./work ./$$impl.temp $$impl test 2>> ./err.log; \
		done; \
		deactivate

update:
	@source ${PYENV}/bin/activate; \
		for impl in aocl sdaccel; do \
			rm -rf $$impl.temp; mkdir -p $$impl.temp; \
			export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/scripts:$$PYTHONPATH; \
			export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/libs/opentuner:$$PYTHONPATH; \
				echo "Updating $$impl"; \
				python ./run_test.py ./work ./$$impl.temp $$impl update 2>> ./err.log; \
		done; \
		deactivate

diff: run;

clean:
	@rm -rf *.temp *.log *.xml .merlin*
