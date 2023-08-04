
SHELL=/bin/bash

PROJ_NAME = templates_v0_general

APP_NAME = mm

CASE = mm_2d_array.cpp


frontend:
	mars_opt ${CASE} -e c -p systolic_frontend -a impl_num=1 -a search_step=1

backend: test config

test:
	mars_opt systolic_array_kernel1.cpp -e c -p systolic_extract |& tee test.log

config: 
	pushd .; cd ~/prog/Merlin_Compiler/trunk/set_env ; source set_root.sh; source set_env.sh; popd; pwd;  perl $(MERLIN_COMPILER_HOME)/optimizers/systolic/scripts/systolic_config.pl $(PROJ_NAME) $(APP_NAME)

flow: frontend backend
	rm -rf step6
	mkdir step6
	cp sys_cfg/Makefile step6/
	cp sys_cfg/setenv_systolic.sh step6/
	mkdir step6/scripts
	cp -r sys_cfg/scripts/* step6/scripts/
	mkdir step6/host
	cp -r sys_cfg/host_cnn/* step6/host/
	cp -r sys_cfg/host_$(APP_NAME)/src/cnn.h step6/host/src/cnn.h
	cp -r sys_cfg/host_$(APP_NAME)/src/cnn.cpp step6/host/src/cnn.cpp
	cp -r sys_cfg/host_$(APP_NAME)/src/c_model.cpp step6/host/src/c_model.cpp
	cp -r sys_cfg/host_$(APP_NAME)/src/c_model.h step6/host/src/c_model.h
	mkdir step6/kernels
	cp -r sys_cfg/kernels/* step6/kernels/
	cd step6; make sim

clean:
	rm -rf step6 sys_cfg *.xml
	rm -f rose* *.o systolic_array_kernel* *.log *.ti *.json .finish_read_files
