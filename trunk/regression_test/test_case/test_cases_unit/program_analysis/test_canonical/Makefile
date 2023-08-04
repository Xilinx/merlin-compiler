test:
	-mars_opt test_canonical/test_canonical_1.cpp -e c -p program_analysis -a test_range 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_canonical/test_canonical_2.cpp -e c -p program_analysis -a test_range 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_canonical/test_canonical_3.cpp -e c -p program_analysis -a test_range 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_canonical/test_canonical_4.cpp -e c -p program_analysis -a test_range 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_canonical/test_canonical_5.cpp -e c -p program_analysis -a test_range 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_canonical/test_canonical_6.cpp -e c -p program_analysis -a test_range 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_canonical/test_canonical_7.cpp -e c -p program_analysis -a test_range 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_canonical/test_canonical_8.cpp -e c -p program_analysis -a test_range 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_canonical/test_canonical_call.cpp -e c -p program_analysis -a test_range 2>tmp.log || (cat tmp.log >> err.log)
	-make diff

diff:
	-diff --brief rose_test_canonical_1.cpp test_canonical_1.out.ref &>> err.log
	-diff --brief rose_test_canonical_2.cpp test_canonical_2.out.ref &>> err.log
	-diff --brief rose_test_canonical_3.cpp test_canonical_3.out.ref &>> err.log
	-diff --brief rose_test_canonical_4.cpp test_canonical_4.out.ref &>> err.log
	-diff --brief rose_test_canonical_5.cpp test_canonical_5.out.ref &>> err.log
	-diff --brief rose_test_canonical_6.cpp test_canonical_6.out.ref &>> err.log
	-diff --brief rose_test_canonical_7.cpp test_canonical_7.out.ref &>> err.log
	-diff --brief rose_test_canonical_8.cpp test_canonical_8.out.ref &>> err.log
	-diff --brief rose_test_canonical_9.cpp test_canonical_9.out.ref &>> err.log
	-diff --brief rose_test_canonical_10.cpp test_canonical_10.out.ref &>> err.log
	-diff --brief rose_test_canonical_call.cpp test_canonical_call.out.ref &>> err.log
	
