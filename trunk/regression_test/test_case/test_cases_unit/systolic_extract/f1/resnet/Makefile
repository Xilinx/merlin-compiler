SHELL := /bin/bash

#MERLIN_SYSTOLIC_HOME = ../../../../../../systolic
MERLIN_SYSTOLIC_HOME = $(MERLIN_COMPILER_HOME)/optimizers/systolic
PROJ_NAME = templates_f1/resnet


hls: copy
	cd step6; source setenv_systolic.sh; cd $(SYSTOLIC_ROOT)/src/kernels; source $(SYSTOLIC_ROOT)/scripts/setenv_sdaccel.sh; vivado_hls -f run_hls.tcl

sdx: copy
	cd step6; source setenv_systolic.sh; source $(SYSTOLIC_ROOT)/scripts/setenv_sdaccel.sh; make run_hw_emu

copy:
	mkdir -p step6
	rm -rf step6/*
	cp -r $(MERLIN_SYSTOLIC_HOME)/$(PROJ_NAME)/* ./step6
