SHELL = /bin/bash

MERLIN_SYSTOLIC_HOME = ../../../../../../systolic
PROJ_NAME = templates_f1/multidie


hls: copy
	cd step6/kernels/c; vivado_hls -f run_hls.tcl

sdx: copy
	cd step6; make run_hw_emu

copy:
	mkdir -p step6
	rm -rf step6/*
	cp -r $(MERLIN_SYSTOLIC_HOME)/$(PROJ_NAME)/* ./step6
