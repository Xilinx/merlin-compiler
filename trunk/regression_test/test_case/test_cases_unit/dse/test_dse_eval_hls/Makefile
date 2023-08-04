PYENV="${MERLIN_COMPILER_HOME}/build/venv"

run:
	@source ${PYENV}/bin/activate; \
		for impl in aocl sdaccel; do \
			rm -rf $$impl.temp; mkdir -p $$impl.temp/code_transform_base; \
			cp work/ori_src_$$impl/directive.xml .; \
			cp -rf work/ori_src_$$impl/* $$impl.temp/code_transform_base/; \
			export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/scripts:$$PYTHONPATH; \
			export PYTHONPATH=${MERLIN_COMPILER_HOME}/optimizers/autodse/libs/opentuner:$$PYTHONPATH; \
				python ./run_test.py $$impl 2>> ./err.log; \
		done; \
		deactivate

update: ;

diff: run;

clean:
	@rm -rf *.temp *.log *.xml .merlin*
