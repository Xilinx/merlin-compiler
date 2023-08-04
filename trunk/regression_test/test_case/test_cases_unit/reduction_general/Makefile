exe_cases = test_plus test_minus test_mult test_div test_cyclic
all_test:   build unit_test
update_ref: update
unit_test: syntax

run:diff_exe
	-mars_opt regression_reduction/test_multi_loop.cpp -e c -p reduction_general -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/test_illegal.cpp -e c -p reduction_general -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/test_pragma.cpp -e c -p reduction_general -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/test_iter.cpp -e c -p reduction_general -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/test_scheme.cpp -e c -p reduction_general -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/test_arg.cpp -e c -p reduction_general -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/test_2.cpp -e c -p reduction_general -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/test_1.cpp -e c -p reduction_general -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/test.c -e c -p reduction_general -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt regression_reduction/test_trip_count.cpp -e c -p midend_preprocess -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt rose_test_trip_count.cpp -e c -p auto_parallel -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt rose_rose_test_trip_count.cpp -e c -p reduction_general -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
	-mv rose_rose_rose_test_trip_count.cpp rose_test_trip_count.final.cpp
	-mars_opt regression_reduction/kmeans/kmeans_kernel.cpp regression_reduction/kmeans/main.cpp \
		-e c -p reduction_general -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)

build:
	make -C ..

syntax:diff
	-gcc -c -std=c99 -fsyntax-only rose_test_multi_loop.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_test_illegal.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_test_pragma.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_test_iter.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_test.c >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_test_1.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_test_2.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_test_arg.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_test_scheme.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only rose_test_trip_count.final.cpp >& tmp.log || (cat tmp.log >> err.log)
	-gcc -c -std=c99 -fsyntax-only -I regression_reduction/kmeans/ rose_kmeans_kernel.cpp >& tmp.log || (cat tmp.log >> err.log)

diff:run
	-diff --brief rose_kmeans_kernel.cpp test_rule/kmeans_kernel.out.ref &>> err.log
	-diff --brief rose_test.c test_rule/test.out.ref &>> err.log
	-diff --brief rose_test_1.cpp test_rule/test_1.out.ref &>> err.log
	-diff --brief rose_test_2.cpp test_rule/test_2.out.ref &>> err.log
	-diff --brief rose_test_arg.cpp test_rule/test_arg.out.ref &>> err.log
	-diff --brief rose_test_scheme.cpp test_rule/test_scheme.out.ref &>> err.log
	-diff --brief rose_test_iter.cpp test_rule/test_iter.out.ref &>> err.log
	-diff --brief rose_test_illegal.cpp test_rule/test_illegal.out.ref &>> err.log
	-diff --brief rose_test_pragma.cpp test_rule/test_pragma.out.ref &>> err.log
	-diff --brief rose_test_multi_loop.cpp test_rule/test_multi_loop.out.ref &>> err.log
	-diff --brief rose_test_trip_count.final.cpp test_rule/test_trip_count.out.ref &>> err.log
	-diff --brief critical_message.rpt test_rule/critical_message.rpt.ref &>> err.log

update:run 
	-cp rose_kmeans_kernel.cpp test_rule/kmeans_kernel.out.ref 
	-cp rose_test.c test_rule/test.out.ref 
	-cp rose_test_1.cpp test_rule/test_1.out.ref 
	-cp rose_test_2.cpp test_rule/test_2.out.ref 
	-cp rose_test_arg.cpp test_rule/test_arg.out.ref 
	-cp rose_test_scheme.cpp test_rule/test_scheme.out.ref 
	-cp rose_test_illegal.cpp test_rule/test_illegal.out.ref 
	-cp rose_test_pragma.cpp test_rule/test_pragma.out.ref 
	-cp rose_test_iter.cpp test_rule/test_iter.out.ref 
	-cp rose_test_multi_loop.cpp test_rule/test_multi_loop.out.ref 
	-cp rose_test_trip_count.final.cpp test_rule/test_trip_count.out.ref 
	-cp critical_message.rpt test_rule/critical_message.rpt.ref 

run_exe:
	-rm -rf *.res *.out *.log *.rpt
	-rm -rf rose*
	-for f in ${exe_cases}; \
	  do \
			mars_opt test_sim/$$f.cpp -e c -p reduction_general -a impl_tool=sdaccel >& tmp1.log || (cat tmp1.log >> err.log); \
			g++ rose_$$f.cpp >& tmp1.log || (cat tmp1.log >> err.log); \
			./a.out  >& tmp1.log || (cat tmp1.log >> err.log); \
            rm a.out; \
	  done
diff_exe:run_exe
	 -for f in ${exe_cases}; \
	  do \
			diff --brief rose_$$f.cpp test_rule/$$f.out.ref &>> err.log; \
	  done
update_exe_ref:run_exe
	 -for f in ${exe_cases}; \
	  do \
	  cp rose_$$f.cpp test_rule/$$f.out.ref; \
	  done
	-cd ../
