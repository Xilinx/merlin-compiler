test:
	-rm -rf err.log
	-touch err.log
	-mars_opt  run_preprocess/sha.c run_preprocess/sha_driver.c  -e c -p preprocess -a naive_hls  -a impl_tool=aocl  -a naive_hls -a debug_mode=debug || echo fails >> err.log
	-diff rose_sha.c sha.out.ref &>> err.log
update:
	-cp rose_sha.c ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_bugzilla/665/sha.out.ref
