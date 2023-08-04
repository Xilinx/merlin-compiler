SHELL=/bin/bash

TEST_SRC = test1.cpp

PROJ_NAME = templates_v0_general

APP_NAME = cnn


extract:
	rm block_wise_reformat.cpp input_h.cpp; rm input_weight.cpp; rm output.cpp; mars_opt $(TEST_SRC) -e c -p systolic_extract |& tee test.log

gdb:
	gdb --args mars_opt_org $(TEST_SRC) -e c -p systolic_extract

setup:  config
	rm -rf export
	mkdir export
	cp -r sys_cfg/* export/

config: extract
	pushd .; cd $(MERLIN_COMPILER_HOME)/set_env ; source set_root.sh; source set_env.sh; popd; pwd;  perl $(MERLIN_COMPILER_HOME)/optimizers/systolic/scripts/systolic_config.pl $(PROJ_NAME) $(APP_NAME)

merlincc: setup
	merlin_backend -flow lib_gen -src export -dst output
	cp -r output/pkg/* ../
	merlin_backend -flow sim_config -src export -dst output
	cp -r output/pkg/* ../

clean:
	rm -rf *.log rose* output export sys_cfg *.xml *.json .finish_read_files ../*.log ../*.so ../*.aocx ../profile.mon ../*.h block_wise_reformat.cpp
