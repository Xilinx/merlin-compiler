PYENV="${MERLIN_COMPILER_HOME}/build/venv"

run:
	@rm -rf *temp
	@echo "Testing config apply with unencrypted source files"; \
	cp -rf src dec.temp; \
	source ${PYENV}/bin/activate; \
		export MERLIN_CURR_PROJECT=`pwd`; \
		cd dec.temp; \
		python ${MERLIN_COMPILER_HOME}/optimizers/autodse/scripts/ds_proc_applier.pyc ds_config.json .; \
		deactivate; \
		cd ../
	@echo "Testing config apply with encrypted source files"; \
	cp -rf src enc.temp; \
	mars_perl -I${MERLIN_COMPILER_HOME}/mars-gen/scripts/lib -Mmerlin -e 'encrypt_dir(@ARGV)' enc.temp; \
	source ${PYENV}/bin/activate; \
		export MERLIN_CURR_PROJECT=`pwd`; \
		cd enc.temp; \
		python ${MERLIN_COMPILER_HOME}/optimizers/autodse/scripts/ds_proc_applier.pyc ds_config.json . --encrypt; \
		deactivate

update: run
	@cp -f dec.temp/__merlinkernel_*.cpp refs/out.cpp

diff: run
	@diff --brief dec.temp/__merlinkernel_*.cpp refs/out.cpp &>> err.log
	@mars_perl -I${MERLIN_COMPILER_HOME}/mars-gen/scripts/lib -Mmerlin -e 'decrypt_dir(@ARGV)' enc.temp; \
		diff --brief enc.temp/__merlinkernel_*.cpp refs/out.cpp &>> err.log

clean:
	find . -name "rose*"  -prune -exec rm -rf {} \;
	find . -name "flex*"  -prune -exec rm -rf {} \;
	find . -name "*.log"  -prune -exec rm -rf {} \;
	find . -name "*.rpt"  -prune -exec rm -rf {} \;
	find . -name "*.temp" -prune -exec rm -rf {} \; 
	find . -name ".*idx*" -prune -exec rm -rf {} \;
