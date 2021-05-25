
run:
	rm -rf host/bin/* ; source ./setenv_systolic.sh ; ./scripts/compile_host.sh ; ./scripts/run_host.sh |& tee log

build_run:
	rm -rf host/bin/* ./*.aocx kernels/matrixMult/*.aocx ; source ./setenv_systolic.sh ; ./scripts/compile_kernels.sh 

build_run_seed:
	rm -rf host/bin/* ./*.aocx ; source ./setenv_systolic.sh ; ./scripts/compile_kernels_seed.sh 1


sim: build_sim
	rm -rf log; source ./setenv_systolic.sh ; ./scripts/compile_host_sim.sh ; ./scripts/run_sim.sh |& tee log
	
build_sim:
	rm -rf host/bin/* ./*.sim.aocx kernels/matrixMult/*.sim.aocx ./*.sim; source ./setenv_systolic.sh; ./scripts/compile_kernels_sim.sh; 

# print out the last processing idx of PE, Drain_C, etc.	
grep_last: scripts/grep_last.cpp
	@source ./setenv_systolic.sh ; ./scripts/compile_grep_last.sh; 
	

#gdb_sim:
#	env CL_CONTEXT_EMULATOR_DEVICE_ALTERA=1 gdb --args ./host/bin/matrixMult_sim
#
#gdb:
#	gdb --args ./host/bin/matrixMult_sim
#

