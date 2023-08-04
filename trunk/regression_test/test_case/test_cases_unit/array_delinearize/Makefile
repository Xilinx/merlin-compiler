unit_test: compare 
    
init:
	rm -rf err.log rose_* *.rpt
	touch err.log
run: init
	make test_delinear
run_delinear: 
	rm -rf rose_test_delinear.cpp
	-cd test_delinear; mars_opt -I ../ test_delinear.cpp -e c -p delinearization || echo $@ fails >> ../err.log; \
     gcc -I ../ -I ${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin -fsyntax-only rose_test_delinear.cpp 2> syntax.log || cat syntax.log >> ../err.log

run_delinear_x: 
	rm -rf rose_test_delinear_x.cpp
	-cd test_delinear_x; mars_opt -I ../ test_delinear_x.cpp -e c -p delinearization \
	  -a impl_tool=sdaccel || echo $@ fails >> ../err.log ; \
	gcc -I ../ -I ${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin -fsyntax-only rose_test_delinear_x.cpp 2> syntax_x.log || cat syntax_x.log >> ../err.log

run_alignment: 
	rm -rf rose_test_alignment.cpp
	-cd test_alignment; mars_opt -I ../ test_alignment.cpp -e c -p delinearization || echo $@ fails >> ../err.log; \
	gcc -I ../ -I ${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin  -fsyntax-only rose_test_alignment.cpp 2> syntax.log || cat syntax.log >> ../err.log; 
run_external_and_local:
	-cd test_both_external_and_local; mars_opt -I ../ test_both_external_and_local.c -e c -p delinearization \
	  || echo $@ fails >> ../err.log; \
	gcc -I ../ -I ${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin  -fsyntax-only rose_test_both_external_and_local.c 2> tmp.log || cat tmp.log >> ../err.log

run_cross_delinear:
	-cd test_cross_delinear; mars_opt -I ../ test_cross_delinear.cpp -e c -p delinearization \
	  || echo $@ fails >> ../err.log; \
	gcc -I ../ -I ${MERLIN_COMPILER_HOME}/mars-gen/lib/merlin  -fsyntax-only rose_test_cross_delinear.cpp 2> tmp.log || cat tmp.log >> ../err.log

gen_ref: update
update: 
	cp test_delinear/rose_test_delinear.cpp test_delinear.out.ref 
	cp test_delinear_x/rose_test_delinear_x.cpp test_delinear_x.out.ref
	cp test_both_external_and_local/rose_test_both_external_and_local.c  test_both_external_and_local.out.ref
	cp test_alignment/rose_test_alignment.cpp test_alignment.out.ref 
	cp test_alignment/critical_message.rpt run_alignment_critical_msg.out.ref
	cp test_cross_delinear/rose_test_cross_delinear.cpp test_cross_delinear.out.ref


test_delinear: run_delinear_x run_delinear run_alignment run_external_and_local run_cross_delinear
	echo "finish run"
compare: run
	diff --brief test_delinear/rose_test_delinear.cpp test_delinear.out.ref  &>> err.log
	diff --brief test_delinear_x/rose_test_delinear_x.cpp test_delinear_x.out.ref &>> err.log
	diff --brief test_both_external_and_local/rose_test_both_external_and_local.c  test_both_external_and_local.out.ref &>> err.log
	diff --brief test_alignment/rose_test_alignment.cpp test_alignment.out.ref  &>> err.log
	diff --brief test_cross_delinear/rose_test_cross_delinear.cpp test_cross_delinear.out.ref  &>> err.log
	diff --brief test_alignment/critical_message.rpt run_alignment_critical_msg.out.ref &>> err.log


