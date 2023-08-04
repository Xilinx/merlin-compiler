TEST_DIR = $(ESLOPT_HOME)/../source-opt/tools/program_analysis/test/
exe_cases = test_canonical_2_exe test_canonical_3_exe test_canonical_4_exe \
						test_canonical_5_exe test_canonical_6_exe test_canonical_7_exe test_canonical_8_exe \
						test_canonical_9_exe test_canonical_10_exe test_canonical_ap_int_exe 
range_list =$(shell ls test_range*.c*) $(shell ls test_canonical/*[^_exe].cpp) 
all_test: compare_unit_test
	@rm -rf rose.log rose_succeed
all_exe_test: diff_exe
unit_test: init 
	make all_test 
	make all_exe_test
	@rm -rf rose.log rose_succeed
init: 
	-rm -rf err.log
	-touch err.log
run:    run_var_type \
		test_range \
		run_range_with_assert \
		run_bound_range \
		run_exp \
		run_access\
		run_var_ref\
		run_dep\
		run_defuse_issue\
		run_type_by_ref\
		run_movable\
	 	run_liveness \
	 	run_index_switch\
	 	run_flatten\
	 	run_is_write\
	 	run_loop_order\
	 	run_recursive_func\
	 	run_recursive_type\
		run_call_graph \
		run_call_path \
		run_split_assign \
		run_split_exp \
		run_ast_dump \
		run_ast_collect \
		run_traverse_goto_stmt \
		run_original_node \
		run_split_inlined_agg_def \
		run_type_transform \
		run_sizeof


clean_ref: 
	rm -rf rose_test_var_type.c test_var_type.out.ref
	rm -rf rose_test_exp.c test_exp.out.ref
	rm -rf rose_test_access.c test_access.out.ref
	rm -rf rose_test_dep.c test_dep.out.ref
	rm -rf rose_test_defuse_issue.c test_defuse_issue.out.ref
	rm -rf rose_test_range.c test_range.out.ref
	rm -rf rose_test_call_graph.c test_call_graph.out.ref
	rm -rf rose_test_range_*.c test_range_*.out.ref
	rm -rf rose_test_liveness.c test_liveness.out.ref
	rm -rf rose_test_var_type.c.log test_var_type.out.ref.log
	rm -rf rose_test_exp.c.log test_exp.out.ref.log
	rm -rf rose_test_access.c.log test_access.out.ref.log
	rm -rf rose_test_dep.c.log test_dep.out.ref.log
	rm -rf rose_test_defuse_issue.c.log test_defuse_issue.out.ref.log
	rm -rf rose_test_range.c.log test_range.out.ref.log
	rm -rf rose_test_call_graph.c.log test_call_graph.out.ref.log
	rm -rf rose_test_range_*.c.log test_range_*.out.ref.log
	rm -rf rose_test_canonical_*.cpp.log test_canonical_*.out.ref.log
	rm -rf rose_test_liveness.c.log test_liveness.out.ref.log
	rm -rf rose_test_dim.cpp.log test_dim.out.ref.log
	rm -rf rose_test_movable.cpp.log test_movable.out.ref.log
	rm -rf rose_test_index_switch.cpp.log test_index_switch.out.ref.log
	rm -rf rose_test_index_switch.cpp.log test_index_switch.out.ref.log
	rm -rf rose_test_flatten.cpp.log test_flatten.out.ref.log
	rm -rf rose_test_is_write.cpp.log test_is_write.out.ref.log
	rm -rf rose_test_is_write.cpp.log test_is_write.out.ref.log
	rm -rf rose_test_loop_order.cpp.log test_loop_order.out.ref.log
	rm -rf rose_test_loop_order.cpp.log test_loop_order.out.ref.log

gen_ref: update_ref update_exe_ref update_range
update_exe_ref:run_exe
update_ref: run 
	-mv rose_test_split_exp.cpp test_split_exp.out.ref 
	-mv rose_test_original_node.cpp.log test_original_node.out.ref.log
	-mv rose_test_split_inlined_agg_def.cpp test_split_inlined_agg_def.out.ref
	-mv rose_test_split_assign.cpp test_split_assign.out.ref 
	-mv rose_test_split_assign.cpp.log test_split_assign.out.ref.log
	-mv rose_test_var_type.c test_var_type.out.ref
	-mv rose_test_ast_collect.cpp.log test_ast_collect.out.ref.log
	-mv rose_test_exp.c test_exp.out.ref
	-mv rose_test_access.c test_access.out.ref
	-mv rose_test_call_graph.c test_call_graph.out.ref
	-mv rose_test_dim.cpp test_dim.out.ref
	-mv rose_test_type_transform.cpp test_type_transform.out.ref
	-mv rose_test_type_transform.cpp.log test_type_transform.out.ref.log
	-mv rose_test_var_type.c.log test_var_type.out.ref.log
	-mv rose_test_exp.c.log test_exp.out.ref.log
	-mv rose_test_exp_with_cond.c.log test_exp_with_cond.out.ref.log
	-mv rose_test_access.c.log test_access.out.ref.log
	-mv rose_test_var_ref.c.log test_var_ref.out.ref.log
	-mv rose_test_dep.c test_dep.out.ref
	-mv rose_test_defuse_issue.c test_defuse_issue.out.ref
	-mv rose_test_call_graph.c.log test_call_graph.out.ref.log
	-mv rose_test_call_path.c.log test_call_path.out.ref.log
	-mv rose_test_dim.cpp.log test_dim.out.ref.log
	-mv rose_test_movable.cpp test_movable.out.ref
	-mv rose_test_movable.cpp.log test_movable.out.ref.log
	-mv rose_test_index_switch.cpp test_index_switch.out.ref
	-mv rose_test_index_switch.cpp.log test_index_switch.out.ref.log
	-mv rose_test_flatten.cpp test_flatten.out.ref
	-mv rose_test_flatten.cpp.log test_flatten.out.ref.log
	-mv rose_test_is_write.cpp test_is_write.out.ref
	-mv rose_test_is_write.cpp.log test_is_write.out.ref.log
	-mv rose_test_loop_order.cpp test_loop_order.out.ref
	-mv rose_test_loop_order.cpp.log test_loop_order.out.ref.log
	-mv rose_test_recursive_func.cpp test_recursive_func.out.ref
	-mv rose_test_recursive_func.cpp.log test_recursive_func.out.ref.log
	-mv rose_test_recursive_type.cpp.log test_recursive_type.out.ref.log
	-mv rose_test_bound_range.c.log test_bound_range.out.ref.log
	-mv rose_test_no_bound_range.c.log test_no_bound_range.out.ref.log
	-mv rose_test_dep.c.log test_dep.out.ref.log
	-mv rose_test_defuse_issue.c.log test_defuse_issue.out.ref.log
	-mv rose_test_liveness.c.log test_liveness.out.ref.log
	-mv rose_test_liveness.c test_liveness.out.ref
	-mv rose_test_assert_range.c.log test_assert_range.out.ref.log
	-mv rose_test_assert_range.c test_assert_range.out.ref
	-mv rose_test_assert_range.cpp.log test_assert_range.out.cpp.ref.log
	-mv rose_test_assert_range.cpp test_assert_range.out.cpp.ref
	-mv rose_test_assert_range_expr.c.log test_assert_range_expr.out.ref.log
	-mv rose_test_assert_range_expr.c test_assert_range_expr.out.ref
	-mv rose_test_ast_dump.cpp.log test_ast_dump.cpp.out.ref.log
	-mv rose_test_traverse_goto_stmt.c.log test_traverse_goto_stmt.out.ref.log
	-mv rose_test_sizeof_ap_int.log test_sizeof_ap_int.ref.log &>> err.log

compare_unit_test: run
	-diff --brief rose_test_var_type.c test_var_type.out.ref &>> err.log
	-diff --brief rose_test_exp.c test_exp.out.ref &>> err.log
	-diff --brief rose_test_access.c test_access.out.ref &>> err.log
	-diff --brief rose_test_dep.c test_dep.out.ref &>> err.log
	-diff --brief rose_test_defuse_issue.c test_defuse_issue.out.ref &>> err.log
	-diff --brief rose_test_call_graph.c test_call_graph.out.ref &>> err.log
	-diff --brief rose_test_dim.cpp test_dim.out.ref &>> err.log
	-diff --brief rose_test_type_transform.cpp test_type_transform.out.ref &>> err.log
	-diff --brief rose_test_type_transform.cpp.log test_type_transform.out.ref.log &>> err.log
	-diff --brief rose_test_assert_range.c test_assert_range.out.ref &>> err.log
	-diff --brief rose_test_assert_range_expr.c test_assert_range_expr.out.ref &>> err.log
	-diff --brief rose_test_assert_range.cpp test_assert_range.out.cpp.ref &>> err.log
	-diff --brief rose_test_movable.cpp test_movable.out.ref &>> err.log
	-diff --brief rose_test_flatten.cpp test_flatten.out.ref &>> err.log
	-diff --brief rose_test_is_write.cpp test_is_write.out.ref &>> err.log
	-diff --brief rose_test_liveness.c test_liveness.out.ref &>> err.log
	-diff --brief rose_test_loop_order.cpp test_loop_order.out.ref &>> err.log
	-diff --brief rose_test_recursive_func.cpp test_recursive_func.out.ref &>> err.log
	-diff --brief rose_test_index_switch.cpp test_index_switch.out.ref &>> err.log
	-diff --brief rose_test_split_assign.cpp test_split_assign.out.ref &>> err.log
	-diff --brief rose_test_split_exp.cpp test_split_exp.out.ref &>> err.log
	-diff --brief rose_test_original_node.cpp.log test_original_node.out.ref.log &>> err.log
	-diff --brief rose_test_split_inlined_agg_def.cpp test_split_inlined_agg_def.out.ref &>> err.log
	-diff --brief rose_test_ast_collect.cpp.log test_ast_collect.out.ref.log &>> err.log
	-diff --brief rose_test_var_type.c.log test_var_type.out.ref.log &>> err.log
	-diff --brief rose_test_exp.c.log test_exp.out.ref.log &>> err.log
	-diff --brief rose_test_exp_with_cond.c.log test_exp_with_cond.out.ref.log &>> err.log
	-diff --brief rose_test_access.c.log test_access.out.ref.log &>> err.log
	-diff --brief rose_test_var_ref.c.log test_var_ref.out.ref.log &>> err.log
	-diff --brief rose_test_dep.c.log test_dep.out.ref.log &>> err.log
	-diff --brief rose_test_ast_dump.cpp.log test_ast_dump.cpp.out.ref.log &>>err.log
	-diff --brief rose_test_defuse_issue.c.log test_defuse_issue.out.ref.log &>> err.log
	-diff --brief rose_test_call_graph.c.log test_call_graph.out.ref.log &>> err.log
	-diff --brief rose_test_call_path.c.log test_call_path.out.ref.log &>> err.log
	-diff --brief rose_test_dim.cpp.log test_dim.out.ref.log &>> err.log
	-diff --brief rose_test_movable.cpp.log test_movable.out.ref.log &>> err.log
	-diff --brief rose_test_split_assign.cpp.log test_split_assign.out.ref.log &>> err.log
	-diff --brief rose_test_index_switch.cpp.log test_index_switch.out.ref.log &>> err.log
	-diff --brief rose_test_is_write.cpp.log test_is_write.out.ref.log &>> err.log
	-diff --brief rose_test_loop_order.cpp.log test_loop_order.out.ref.log &>> err.log
	-diff --brief rose_test_recursive_func.cpp.log test_recursive_func.out.ref.log &>> err.log
	-diff --brief rose_test_recursive_type.cpp.log test_recursive_type.out.ref.log &>> err.log
	-diff --brief rose_test_liveness.c.log test_liveness.out.ref.log &>> err.log
	-diff --brief rose_test_bound_range.c.log test_bound_range.out.ref.log &>> err.log
	-diff --brief rose_test_no_bound_range.c.log test_no_bound_range.out.ref.log &>> err.log
	-diff --brief rose_test_assert_range.c.log test_assert_range.out.ref.log &>> err.log
	-diff --brief rose_test_assert_range_expr.c.log test_assert_range_expr.out.ref.log &>> err.log
	-diff --brief rose_test_assert_range.cpp.log test_assert_range.out.cpp.ref.log &>> err.log
	-diff --brief rose_test_flatten.cpp.log test_flatten.out.ref.log &>> err.log
	-diff --brief rose_test_traverse_goto_stmt.c.log test_traverse_goto_stmt.out.ref.log &>> err.log
	-diff --brief rose_test_sizeof_ap_int.log test_sizeof_ap_int.ref.log &>> err.log
run_ast_collect: 
	rm -rf rose_test_ast_collect.cpp
	-mars_opt test_ast_collect.cpp -e c -p program_analysis -a test_ast_collect | tee rose_test_ast_collect.cpp.log

run_traverse_goto_stmt:
	rm -rf rose_test_traverse_goto_stmt.c
	-mars_opt test_traverse_goto_stmt.c -e c -p program_analysis -a test_traverse_all_nodes | tee rose_test_traverse_goto_stmt.c.log

run_var_type: 
	rm -rf rose_test_var_type.c
	-mars_opt test_var_type.c -e c -p program_analysis -a test_var_type | tee rose_test_var_type.c.log

run_exp: 
	rm -rf rose_test_exp.c
	-mars_opt test_exp.c -e c -p program_analysis -a test_exp | tee rose_test_exp.c.log
	rm -rf rose_test_exp_with_cond.c
	-mars_opt test_exp_with_cond.c -e c -p program_analysis -a test_exp | tee rose_test_exp_with_cond.c.log

run_access: 
	rm -rf rose_test_access.c
	-mars_opt test_access.c -e c -p program_analysis -a test_access | tee rose_test_access.c.log

run_var_ref: 
	rm -rf rose_test_var_ref.c
	-mars_opt test_var_ref.c -e c -p program_analysis -a test_var_ref | tee rose_test_var_ref.c.log

run_dep: 
	rm -rf rose_test_dep.c
	-mars_opt test_dep.c -e c -p program_analysis -a test_dependence | tee rose_test_dep.c.log

run_defuse_issue: 
	rm -rf rose_test_defuse_issue.c
	-mars_opt test_defuse_issue.c -e c -p midend_preprocess -a test_defuse_issue| tee rose_test_defuse_issue.c.log

run_range:
	for f in $(range_list); \
	  do  \
	   bn=`basename $$f`;\
	   echo $$f; \
	   rm -rf rose_$$f; \
	mars_opt $${f} -e c -p program_analysis -a test_range -I include | tee tmp_rose_$${bn}.log; \
	grep -v __leaf__  tmp_rose_$${bn}.log > rose_$${bn}.log; \
	done


run_range_with_assert: 
	rm -rf rose_test_assert_range.c
	-mars_opt test_assert_range.c -e c -p program_analysis -a test_assert_range | tee tmp_rose_test_assert_range.c.log
	grep -v __leaf__  tmp_rose_test_assert_range.c.log > rose_test_assert_range.c.log
	rm -rf rose_test_assert_range.cpp
	-mars_opt test_assert_range.cpp -e c -p program_analysis -a test_assert_range | tee tmp_rose_test_assert_range.cpp.log
	grep -v __leaf__  tmp_rose_test_assert_range.cpp.log > rose_test_assert_range.cpp.log
	rm -rf rose_test_assert_range_expr.c
	-mars_opt test_assert_range_expr.c -e c -p program_analysis -a test_assert_range | tee tmp_rose_test_assert_range_expr.c.log
	grep -v __leaf__  tmp_rose_test_assert_range_expr.c.log > rose_test_assert_range_expr.c.log
	-mars_opt test_assert_range_loop_with_recursive_host.c -e c -p program_analysis -a test_assert_range | tee tmp_rose_test_assert_range_loop_with_recursive_host.c.log
	grep -v __leaf__  tmp_rose_test_assert_range_loop_with_recursive_host.c.log > rose_test_assert_range_loop_with_recursive_host.c.log

run_bound_range:
	rm -rf rose_test_bound_range.c
	-mars_opt test_bound_range.c -e c -p program_analysis -a test_bound_range | tee rose_test_bound_range.c.log
	-mars_opt test_bound_range.c -e c -p program_analysis -a test_range | tee rose_test_no_bound_range.c.log
	

run_call_graph: 
	rm -rf rose_test_call_graph.c
	-mars_opt test_call_graph.c -e c -p program_analysis -a test_call_graph | tee rose_test_call_graph.c.log

run_call_path: 
	rm -rf rose_test_call_path.c
	-mars_opt test_call_path.c -e c -p program_analysis -a test_call_path | tee rose_test_call_path.c.log

run_type_by_ref: 
	rm -rf rose_test_dim.cpp
	-mars_opt test_dim.cpp -e c -p program_analysis -a test_type_by_ref| tee rose_test_dim.cpp.log

run_type_transform: 
	rm -rf rose_test_type_transform.cpp
	-mars_opt test_type_transform.cpp -e c -p program_analysis -a test_type_transform| tee rose_test_type_transform.cpp.log

run_movable: 
	rm -rf rose_test_movable.cpp
	-mars_opt test_movable.cpp -e c -p program_analysis -a test_movable | tee rose_test_movable.cpp.log

run_index_switch: 
	rm -rf rose_test_index_switch.cpp
	-mars_opt test_index_switch.cpp -e c -p program_analysis -a test_index_switch | tee rose_test_index_switch.cpp.log

run_flatten: 
	rm -rf rose_test_flatten.cpp
	-mars_opt test_flatten.cpp -e c -p program_analysis -a test_flatten | tee rose_test_flatten.cpp.log

run_is_write: 
	rm -rf rose_test_is_write.cpp
	-mars_opt test_is_write.cpp -e c -p program_analysis -a test_is_write | tee rose_test_is_write.cpp.log

run_loop_order: 
	rm -rf rose_test_loop_order.cpp
	-mars_opt test_loop_order.cpp -e c -p program_analysis -a test_loop_order | tee rose_test_loop_order.cpp.log

run_recursive_func: 
	rm -rf rose_test_recursive_func.cpp
	-mars_opt test_recursive_func.cpp -e c -p program_analysis -a test_recursive_func | tee rose_test_recursive_func.cpp.log

run_recursive_type: 
	rm -rf rose_test_recursive_type.cpp
	-mars_opt test_recursive_type.cpp -e c -p program_analysis -a test_recursive_type | tee rose_test_recursive_type.cpp.log


run_liveness: 
	rm -rf rose_test_liveness.c
	-mars_opt test_liveness.c -e c -p program_analysis -a test_liveness | tee rose_test_liveness.c.log

run_split_assign: 
	rm -rf rose_test_split_assign.cpp
	-mars_opt test_split_assign.cpp -e c -p program_analysis -a test_split_assign | tee rose_test_split_assign.cpp.log

run_split_exp: 
	rm -rf rose_test_split_exp.cpp
	-mars_opt test_split_exp.cpp -e c -p program_analysis -a test_split_exp | tee rose_test_split_exp.cpp.log

run_ast_dump:
	rm -rf rose_test_ast_dump.cpp
	-mars_opt test_ast_dump.cpp -e c -p program_analysis -a ast_dump |& tail -25 | tee rose_test_ast_dump.cpp.log

run_original_node:
	rm -rf rose_test_original_node.cpp
	-mars_opt test_original_node.cpp -e c -p program_analysis -a test_original_node | tee rose_test_original_node.cpp.log

run_split_inlined_agg_def:
	rm -rf rose_test_split_inlined_agg_def.cpp
	-mars_opt test_split_inlined_agg_def.cpp -e c -p program_analysis -a test_split_inlined_agg_def | tee rose_test_split_inlined_agg_def.cpp.log

run_sizeof:
	rm -rf rose_test_sizeof.cpp
	-mars_opt test_sizeof.cpp -e c -p program_analysis -a test_sizeof_ap_int | tee rose_test_sizeof_ap_int.log
run_exe:
	-rm -rf *.res
	-for f in ${exe_cases}; \
	  do \
			g++ test_canonical/$$f.cpp; \
			./a.out | tee $$f.res; \
			mars_opt test_canonical/$$f.cpp -e c -p program_analysis -a test_range; \
			g++ rose_$$f.cpp; \
			./a.out | tee rose_$$f.res; \
	  done
diff_exe:run_exe 
	-for f in ${exe_cases}; \
	  do \
			diff --brief rose_$$f.cpp test_canonical/$$f.out.ref &>> err.log; \
	    diff --brief $$f.res rose_$$f.res &>> err.log; \
	  done
update_exe_ref: run_exe
	-for f in ${exe_cases}; \
	  do \
	  cp rose_$$f.cpp test_canonical/$$f.out.ref; \
	  done

test_var_type: run_var_type
	-diff --brief rose_test_var_type.c test_var_type.out.ref
	-diff --brief rose_test_var_type.c.log test_var_type.out.ref.log

test_exp: run_exp
	-diff --brief rose_test_exp.c test_exp.out.ref
	-diff --brief rose_test_exp.c.log test_exp.out.ref.log

test_access: run_access
	-diff --brief rose_test_access.c test_access.out.ref
	-diff --brief rose_test_access.c.log test_access.out.ref.log

test_dep: run_dep
	-diff --brief rose_test_dep.c test_dep.out.ref
	-diff --brief rose_test_dep.c.log test_dep.out.ref.log

test_defuse_issue: run_defuse_issue
	-diff --brief rose_test_defuse_issue.c test_defuse_issue.out.ref
	-diff --brief rose_test_defuse_issue.c.log test_defuse_issue.out.ref.log

test_range: run_range
	-for f in ${range_list}; \
	  do \
	  bf=`basename $${f}`; \
	  bn=`basename $${f} .c`; \
	  bn=`basename $${bn} .cpp`; \
	  diff --brief rose_$${bf}.log $${bn}.out.ref.log  &>> err.log; \
	  diff --brief rose_$${bf} $${bn}.out.ref  &>> err.log; \
	  done
	-grep "src_trip_count" history_program_analysis.json 2>tmp.log || (echo "trip count msg not found">> err.log && cat tmp.log >> err.log) 
	echo "test range"

update_range: run_range
	-for f in ${range_list}; \
	  do \
	  bf=`basename $${f}`; \
	  bn=`basename $${f} .c`; \
	  bn=`basename $${bn} .cpp`; \
	  cp rose_$${bf}.log $${bn}.out.ref.log ; \
	  cp rose_$${bf} $${bn}.out.ref; \
	  done
	echo "update range"

test_call_graph: run_call_graph
	-diff --brief rose_test_call_graph.c test_call_graph.out.ref
	-diff --brief rose_test_call_graph.c.log test_call_graph.out.ref

test_range_with_assert: run_range_with_assert
	-diff --brief rose_test_assert_range.c.log test_assert_range.out.ref.log
	-diff --brief rose_test_assert_range.c test_assert_range.out.ref
	-diff --brief rose_test_assert_range.cpp.log test_assert_range.out.cpp.ref.log
	-diff --brief rose_test_assert_range.cpp test_assert_range.out.cpp.ref

test_dim: run_type_by_ref
	-diff --brief rose_test_dim.cpp test_dim.out.ref
	-diff --brief rose_test_dim.cpp.log test_dim.out.ref.log

test_movable: run_movable
	-diff --brief rose_test_movable.cpp test_movable.out.ref
	-diff --brief rose_test_movable.cpp.log test_movable.out.ref.log

test_index_switch: run_index_switch
	-diff --brief rose_test_index_switch.cpp test_index_switch.out.ref
	-diff --brief rose_test_index_switch.cpp.log test_index_switch.out.ref.log

test_flatten: run_flatten
	-diff --brief rose_test_flatten.cpp test_flatten.out.ref

test_is_write: run_is_write
	-diff --brief rose_test_is_write.cpp test_is_write.out.ref
	-diff --brief rose_test_is_write.cpp.log test_is_write.out.ref.log

test_loop_order: run_loop_order
	-diff --brief rose_test_loop_order.cpp test_loop_order.out.ref
	-diff --brief rose_test_loop_order.cpp.log test_loop_order.out.ref.log

test_recursive_func: run_recursive_func
	-diff --brief rose_test_recursive_func.cpp test_recursive_func.out.ref
	-diff --brief rose_test_recursive_func.cpp.log test_recursive_func.out.ref.log

test_liveness: run_liveness
	diff --brief rose_test_liveness.c test_liveness.out.ref
	diff --brief rose_test_liveness.c.log test_liveness.out.ref.log

test_split_assign: run_split_assign
	-diff --brief rose_test_split_assign.cpp   test_split_assign.out.ref

test_split_exp: run_split_exp
	-diff --brief rose_test_split_exp.cpp   test_split_exp.out.ref

test_original_node: run_original_node
	-diff --brief rose_test_original_node.cpp.log   test_original_node.out.ref.log

test_split_inlined_agg_def: run_split_inlined_agg_def
	-diff --brief rose_test_split_inlined_agg_def.cpp   test_split_inlined_agg_def.out.ref

build: 
	make -C ..

debug: 
	gdb --args mars_opt xxx.c -e c -p program_analysis -a ...
