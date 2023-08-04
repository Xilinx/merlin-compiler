test:
	-rm -rf err.log
	touch err.log
	-mars_opt  run_comm_sync/host.c run_comm_sync/idct_kernel.c run_comm_sync/__merlinwrapper_idct_kernel.c  \
	-e c -p comm_sync  -a impl_tool=aocl  -a debug_mode=debug  || echo fails >> err.log
	-diff rose_idct_kernel.c idct_kernel.out.ref &>> err.log
update:
	-cp rose_idct_kernel.c ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_bugzilla/740/idct_kernel.out.ref
