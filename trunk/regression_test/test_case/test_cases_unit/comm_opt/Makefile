
err: build
#	mars_opt test/run_comm_sync_cnn/main.cpp test/run_comm_sync_cnn/cnn_kernel.cpp \
		-e c -p comm_sync  -a debug_mode=debug | tee rose_err.log
#	mars_opt test/run_comm_refine/namd_kernel.c test/run_comm_refine/namd.c \
		-e c -p comm_refine  -a debug_mode=debug | tee rose_err.log
#	mars_opt test/run_comm_sync/matmul.cpp test/run_comm_sync/matmul_kernel.cpp \
		-e c -p comm_sync  -a debug_mode=debug | tee rose_err.log
#	mars_opt test/run_comm_refine/logistic.c test/run_comm_refine/logistic_regression_kernel.c \
	-e c -p comm_refine  -a debug_mode=debug | tee rose_ernamdg
#	mars_opt  test/run_comm_refine/idct_kernel.c test/run_comm_refine/host.c  \
	-e c -p comm_refine  -a debug_mode=debug | tee rose_err.log
	mars_opt test/run_comm_token/logistic.cpp test/run_comm_token/logistic_regression_kernel.cpp \
	-e c -p comm_token  -a debug_mode=debug | tee rose_err.log
#	mars_opt  test/run_comm_token/host.cpp test/run_comm_token/run_kernel.cpp  \
	-e c -p comm_token  -a debug_mode=debug | tee rose_err.log
#	mars_opt test/testerr.c -e c -p comm_sync | tee rose_err.log
#	mars_opt test/testerr.c -e c -p comm_refine | tee rose_err.log
#	mars_opt test/testerr.c -e c -p comm_token | tee rose_err.log
#	mars_opt test/run_comm_token/host.cpp test/run_comm_token/test_kernel.cpp \
		-e c -p comm_token | tee rose_err.log
#	mars_opt test/run_comm_opt/main.cpp test/run_comm_opt/cnn_kernel.cpp \
		-e c -p comm_opt | tee rose_err.log

build:
	make -C ..

unit_test: init_test
	make all_test
all_test: test_refine  test_rename test_token test_opt test_bugs test_sync test_false_dep test_reorder test_check test_dead
update_ref: update_refine  update_rename update_token update_opt update_sync update_false_dep update_reorder update_dead update_check

init_test:
	-rm -rf err.log rose*
test_refine: compare_refine
test_rename: compare_rename
test_token: compare_token
test_opt: compare_opt
test_sync: compare_sync
test_bugs: compare_bugs
test_false_dep: compare_false_dep
test_reorder: compare_reorder
test_check: compare_check 
test_dead: compare_dead
run_refine:
	-cd test_refine/logistic_L10 && mars_opt  host.cpp gradient.cpp  \
		-e c -p comm_refine -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-cd test_refine/swm && mars_opt  smith_waterman.c smithwaterman_kernel.c  \
		-e c -p comm_refine  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-cd test_refine/matmul && mars_opt  matmul.cpp matmul_kernel.cpp  \
		-e c -p comm_refine  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-cd test_refine/normal  && mars_opt  normal_kernel.cpp testNormalC.cpp \
		-e c -p comm_refine  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-cd test_refine/cnn && mars_opt cnn_kernel.cpp \
		-e c -p comm_refine  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-cd test_refine/logistic_16 && mars_opt logistic.c logistic_regression_kernel.c  \
		-e c -p comm_refine  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-cd test_refine/namd &&  mars_opt namd_kernel.c namd.c  \
		-e c -p comm_refine  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-cd test_refine/idct && mars_opt  idct_kernel.c host.c  \
		-e c -p comm_refine -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)

run_rename:
	-cd test_rename/kmeans && mars_opt __merlin_api_list.cpp __merlinwrapper_kmeans.cpp \
      baseline.cpp host.cpp kmeans.cpp  -e c -p comm_rename 2>tmp.log || \
	  (cat tmp.log >> err.log)
	-cd test_rename/logistic && mars_opt logistic.c logistic_regression_kernel.c -e c \
	  -p comm_rename 2> tmp.log || cat tmp.log >> err.log

run_sync:
	-cd test_sync/cnn && mars_opt cnn_kernel.cpp \
		-e c -p comm_sync  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-cd test_sync/logistic && mars_opt logistic.c logistic_regression_kernel.c \
		-e c -p comm_sync  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-cd test_sync/idct && mars_opt  host.c idct_kernel.c  \
	__merlinwrapper_idct_kernel.c -e c -p comm_sync  -a impl_tool=aocl  \
	-a debug_mode=debug  2> tmp.log || cat tmp.log >> err.log

run_opt:
	-cd test_opt/vec_add && mars_opt vec_add.c vec_add_kernel.c -e c -p comm_opt \
	  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-cd test_opt/logistic &&  mars_opt  \
	  gradient.cpp host.cpp -a false_data_dep=on  -e c -p comm_opt  \
	  -a debug_mode=debug 2> tmp.log || cat tmp.log >> err.log
	-cd test_opt/logistic_64 && mars_opt logistic.c logistic_regression_kernel.c  \
		  -e c -p comm_opt -a false_data_dep=on 2>tmp.log || cat tmp.log >> err.log


run_reorder:
	-cd test_reorder/vec_add && mars_opt vec_add.c vec_add_kernel.c -e c -p comm_opt -a channel_reorder \
	  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)

run_token:
	-rm -rf critical_message.rpt
	-cd test_token/matmul && mars_opt matmul_kernel.cpp matmul.cpp  \
		-e c -p comm_token  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-cd test_token/cnn && mars_opt cnn_kernel.cpp  \
		-e c -p comm_token  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-cd test_token/two_kernel && mars_opt  host.cpp two_kernel.cpp  \
		-e c -p comm_token  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt  test_token/test.cpp  \
		-e c -p comm_token  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt  test_token/test2.cpp  \
		-e c -p comm_token  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt  test_token/test3.cpp  \
		-e c -p comm_token  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt  test_token/test4.cpp  \
		-e c -p comm_token  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt  test_token/test5.cpp  \
		-e c -p comm_token  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt  test_token/test_err.cpp  \
		-e c -p comm_token  -a debug_mode=debug
	-cd test_token/logistic_16 && mars_opt logistic.cpp logistic_regression_kernel.cpp \
		-e c -p comm_token  -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)

run_bugs:
	-cd test_bugs/359 && mars_opt testerr.c -e c -p coarse_pipeline_altera 2> tmp.log || cat tmp.log >> err.log
	-cd test_bugs/391 && mars_opt testerr.c -e c -p comm_token 2> tmp.log || cat tmp.log >> err.log
run_false_dep:
	-cd test_false_dep && mars_opt false_dep_0.cpp -e c -p comm_opt -a false_data_dep=off  2> tmp.log || cat tmp.log >> err.log
	-cd test_false_dep && mars_opt false_dep_1.cpp -e c -p comm_opt -a false_data_dep=on  2> tmp.log || cat tmp.log >> err.log
	-cd test_false_dep && mars_opt false_dep_2.cpp -e c -p comm_opt  2> tmp.log || cat tmp.log >> err.log
	
run_check:
	-cd test_check/logistic &&  mars_opt  \
	  gradient.cpp host.cpp -a false_data_dep=on  -e c -p comm_check  \
	  -a debug_mode=debug 2> tmp.log || cat tmp.log >> err.log
	-cd test_check/logistic_64 && mars_opt logistic.c logistic_regression_kernel.c  \
		  -e c -p comm_check -a false_data_dep=on 2>tmp.log || cat tmp.log >> err.log


run_dead:
	-mars_opt  test_dead/test_dead_removal.cpp  \
		-e c -p comm_check -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt  test_dead/test_dead_removal.cpp  \
		-e c -p comm_dead_remove -a debug_mode=debug 2>tmp.log || (cat tmp.log >> err.log)

compare_refine: run_refine
	-diff --brief test_refine/logistic_L10/rose_gradient.cpp test_rule/refine/gradient.ref &>> err.log
	-diff --brief test_refine/swm/rose_smithwaterman_kernel.c test_rule/refine/smithwaterman.ref &>> err.log
	-diff --brief test_refine/matmul/rose_matmul_kernel.cpp test_rule/refine/matmul.ref &>> err.log
	-diff --brief test_refine/normal/rose_normal_kernel.cpp test_rule/refine/normal.ref &>> err.log
	-diff --brief test_refine/cnn/rose_cnn_kernel.cpp test_rule/refine/cnn.ref &>> err.log
	-diff --brief test_refine/logistic_16/rose_logistic_regression_kernel.c test_rule/refine/logistic.ref &>> err.log
	-diff --brief test_refine/namd/rose_namd_kernel.c test_rule/refine/namd.ref &>> err.log
	-diff --brief test_refine/idct/rose_idct_kernel.c test_rule/refine/idct.ref &>> err.log

compare_token: run_token
	-diff --brief test_token/matmul/rose_matmul_kernel.cpp test_rule/token/matmul_kernel.ref &>> err.log
	-diff --brief test_token/cnn/rose_cnn_kernel.cpp test_rule/token/cnn_kernel.ref &>> err.log
	-diff --brief test_token/two_kernel/rose_two_kernel.cpp test_rule/token/two_kernel.ref &>> err.log
	-diff --brief test_token/logistic_16/rose_logistic_regression_kernel.cpp test_rule/token/logistic_16.ref &>> err.log
	-diff --brief rose_test.cpp test_rule/token/test.ref &>> err.log
	-diff --brief rose_test2.cpp test_rule/token/test2.ref &>> err.log
	-diff --brief rose_test3.cpp test_rule/token/test3.ref &>> err.log
	-diff --brief rose_test4.cpp test_rule/token/test4.ref &>> err.log
	-diff --brief rose_test5.cpp test_rule/token/test5.ref &>> err.log
	-diff --brief critical_message.rpt test_rule/token/critical_message.rpt.ref &>> err.log

compare_rename: run_rename
	-diff --brief test_rename/kmeans/rose_kmeans.cpp test_rule/rename/kmeans.ref &>> err.log
	-diff --brief test_rename/logistic/rose_logistic_regression_kernel.c test_rule/rename/logistic_regression_kernel.ref &>> err.log

compare_opt: run_opt
	-diff --brief test_opt/vec_add/rose_vec_add_kernel.c test_rule/opt/vec_add/vec_add_kernel.ref &>> err.log
	-diff --brief test_opt/logistic/rose_gradient.cpp test_rule/opt/logistic/gradient.ref &>> err.log
	-diff --brief test_opt/logistic_64/rose_logistic_regression_kernel.c test_rule/opt/logistic_64/logistic_regression_kernel.ref &>> err.log

compare_check: run_check
	-diff --brief test_check/logistic/rose_gradient.cpp test_rule/check/logistic/gradient.ref &>> err.log
	-diff --brief test_check/logistic_64/rose_logistic_regression_kernel.c test_rule/check/logistic_64/logistic_regression_kernel.ref &>> err.log

compare_sync: run_sync
	-diff --brief test_sync/cnn/rose_cnn_kernel.cpp test_rule/sync/cnn_kernel.ref &>> err.log
	-diff --brief test_sync/logistic/rose_logistic_regression_kernel.c test_rule/sync/logistic_regression_kernel.ref &>> err.log
	-diff --brief test_sync/idct/rose_idct_kernel.c test_rule/sync/idct_kernel.out.ref &>> err.log

compare_bugs:

compare_false_dep: run_false_dep
	-diff --brief test_false_dep/rose_false_dep_0.cpp test_rule/false_dep/false_dep_0.ref &>> err.log
	-diff --brief test_false_dep/rose_false_dep_1.cpp test_rule/false_dep/false_dep_1.ref &>> err.log
	-diff --brief test_false_dep/rose_false_dep_2.cpp test_rule/false_dep/false_dep_2.ref &>> err.log
compare_reorder: run_reorder
	-diff --brief test_reorder/vec_add/rose_vec_add_kernel.c test_rule/reorder/vec_add/vec_add_kernel.ref &>> err.log

compare_dead: run_dead
	-diff --brief rose_test_dead_removal.cpp test_rule/test_dead_removal.ref &>> err.log


update_refine: run_refine
	-cp -f test_refine/logistic_L10/rose_gradient.cpp test_rule/refine/gradient.ref
	-cp -f test_refine/swm/rose_smithwaterman_kernel.c test_rule/refine/smithwaterman.ref
	-cp -f test_refine/matmul/rose_matmul_kernel.cpp test_rule/refine/matmul.ref
	-cp -f test_refine/normal/rose_normal_kernel.cpp test_rule/refine/normal.ref
	-cp -f test_refine/cnn/rose_cnn_kernel.cpp test_rule/refine/cnn.ref
	-cp -f test_refine/logistic_16/rose_logistic_regression_kernel.c test_rule/refine/logistic.ref
	-cp -f test_refine/namd/rose_namd_kernel.c test_rule/refine/namd.ref
	-cp -f test_refine/idct/rose_idct_kernel.c test_rule/refine/idct.ref

update_token: run_token
	-cp -f test_token/matmul/rose_matmul_kernel.cpp test_rule/token/matmul_kernel.ref
	-cp -f test_token/cnn/rose_cnn_kernel.cpp test_rule/token/cnn_kernel.ref
	-cp -f test_token/two_kernel/rose_two_kernel.cpp test_rule/token/two_kernel.ref
	-cp -f test_token/logistic_16/rose_logistic_regression_kernel.cpp test_rule/token/logistic_16.ref
	-cp -f rose_test.cpp test_rule/token/test.ref
	-cp -f rose_test2.cpp test_rule/token/test2.ref
	-cp -f rose_test3.cpp test_rule/token/test3.ref
	-cp -f rose_test4.cpp test_rule/token/test4.ref
	-cp -f rose_test5.cpp test_rule/token/test5.ref
	-cp -f critical_message.rpt test_rule/token/critical_message.rpt.ref

update_rename: run_rename
	-cp -f test_rename/kmeans/rose_kmeans.cpp test_rule/rename/kmeans.ref
	-cp -f test_rename/logistic/rose_logistic_regression_kernel.c test_rule/rename/logistic_regression_kernel.ref

update_opt: run_opt
	-cp test_opt/vec_add/rose_vec_add_kernel.c test_rule/opt/vec_add/vec_add_kernel.ref
	-cp test_opt/logistic/rose_gradient.cpp test_rule/opt/logistic/gradient.ref 
	-cp test_opt/logistic_64/rose_logistic_regression_kernel.c test_rule/opt/logistic_64/logistic_regression_kernel.ref

update_check: run_check
	-cp test_check/logistic/rose_gradient.cpp test_rule/check/logistic/gradient.ref 
	-cp test_check/logistic_64/rose_logistic_regression_kernel.c test_rule/check/logistic_64/logistic_regression_kernel.ref

update_sync: run_sync
	-cp test_sync/cnn/rose_cnn_kernel.cpp test_rule/sync/cnn_kernel.ref
	-cp test_sync/idct/rose_idct_kernel.c test_rule/sync/idct_kernel.out.ref 
	-cp test_sync/logistic/rose_logistic_regression_kernel.c test_rule/sync/logistic_regression_kernel.ref

update_false_dep: run_false_dep
	-cp test_false_dep/rose_false_dep_0.cpp test_rule/false_dep/false_dep_0.ref 
	-cp test_false_dep/rose_false_dep_1.cpp test_rule/false_dep/false_dep_1.ref 
	-cp test_false_dep/rose_false_dep_2.cpp test_rule/false_dep/false_dep_2.ref 
update_reorder: run_reorder
	-cp test_reorder/vec_add/rose_vec_add_kernel.c test_rule/reorder/vec_add/vec_add_kernel.ref
update_dead: run_dead
	-cp rose_test_dead_removal.cpp test_rule/test_dead_removal.ref
