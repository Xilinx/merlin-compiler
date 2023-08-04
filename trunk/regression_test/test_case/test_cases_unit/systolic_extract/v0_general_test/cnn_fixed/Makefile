SHELL=/bin/bash

TEST_SRC = cnn_small.cpp

PROJ_NAME = templates_v0_general

APP_NAME = cnn

#build: 
#	cd ~/WORK; source ~/mars_setenv.csh; make all; 

build: 
	cd ~/prog/Merlin_Compiler/trunk; cd set_env; source set_root.csh; cd -; make all


test:
	mars_opt $(TEST_SRC) -e c -p systolic_extract |& tee test.log

gdb:
	source ~/fcs_setting64.csh; gdb --args mars_opt $(TEST_SRC) -e c -p systolic_extract

flow:  config
	rm -rf step6
	mkdir step6
	cp sys_cfg/Makefile step6/
	cp sys_cfg/setenv_systolic.sh step6/
	mkdir step6/scripts
	cp -r sys_cfg/scripts/* step6/scripts/
	mkdir step6/host
	cp -r sys_cfg/host_$(APP_NAME)/* step6/host/
	mkdir step6/kernels
	cp -r sys_cfg/kernels/* step6/kernels/
	cd step6; make sim |& tee sim.log

#config: 
#	pushd .; cd ~/WORK; source ~/mars_setenv.csh; popd; pwd; perl $(MERLIN_COMPILER_HOME)/optimizers/systolic/scripts/systolic_config.pl $(PROJ_NAME)

#config: 
#	perl $(MERLIN_COMPILER_HOME)/optimizers/systolic/scripts/systolic_config.pl $(PROJ_NAME) $(APP_NAME)

config: 
	pushd .; cd ~/prog/Merlin_Compiler/trunk/set_env ; source set_root.sh; source set_env.sh; popd; pwd;  perl $(MERLIN_COMPILER_HOME)/optimizers/systolic/scripts/systolic_config.pl $(PROJ_NAME) $(APP_NAME)

clean:
	rm -rf *.log rose* step6 sys_cfg *.xml *.json
