SHELL=/bin/bash

TEST_SRC = test.cpp

PROJ_NAME = templates_v0_general

APP_NAME = mm

extract:
	rm -f block_wise_reformat.cpp; mars_opt $(TEST_SRC) -e c -p systolic_extract |& tee test.log

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


extract_unit_test:extract
	@rm -rf err.log
	@if [ `comm -3 block_wise_reformat.cpp golden_reformat.cpp | wc | awk '{sum+=$$1}END{print sum}'` -eq 0 ];\
	then \
		touch err.log; echo "extract test PASSED! ..."; \
	else \
		echo "`pwd` extract test ERROR! ..." >> err.log; \
	fi;

generate_golden_extract:extract
	@cp block_wise_reformat.cpp golden_reformat.cpp

clean:
	rm -rf *.log rose* output export sys_cfg *.xml *.json .finish_read_files ../*.log ../*.so ../*.aocx ../profile.mon ../*.h block_wise_reformat.cpp
