correct_dir=change_id_not_in_org trip_count unroll require_id_not_exist_in_org_json \
			require_attribute_not_exist_in_org_json delete_some_attributes \
			innormal_attribute non_canonical_loop unroll_org_and_new_full unroll_target_IL_0 \
			unroll_impact_parent_II

incorrect_dir=not_fully_filled unroll_new_full unroll_org_full

gen_ref: run update
unit_test: run diff 
run:
	-rm -rf err.log
	-touch err.log
	-for name in `echo $(correct_dir)`; \
	do \
		cp -r common/lc $$name; \
		cp common/directive.xml $$name; \
		cp common/kernel_list.json $$name/lc; \
		cp common/kernel_file.json $$name/lc; \
		cp common/altera_const.h $$name; \
		cp common/merlin_type_define.h $$name; \
		cp common/run.sh $$name; \
		cd $$name; rm -rf err.log; cd -; \
		cd $$name; cp __merlinkernel_vec_add_kernel.cpp lc; cd -; \
		cd $$name; sh run.sh; cd -; \
	done
	-for name in `echo $(incorrect_dir)`; \
	do \
		cp -r common/lc $$name; \
		cp common/directive.xml $$name; \
		cp common/kernel_list.json $$name/lc; \
		cp common/kernel_file.json $$name/lc; \
		cp common/altera_const.h $$name; \
		cp common/merlin_type_define.h $$name; \
		cp common/run.sh $$name; \
		cd $$name; rm -rf err.log; cd -; \
		cd $$name; cp __merlinkernel_vec_add_kernel.cpp lc; cd -; \
		cd $$name; sh run.sh; cd -; \
	done

update:
	-for name in $(correct_dir); \
	do \
		cd $$name; cp perf_est.json  ref_perf_est.json; cd -; \
	done
	
diff:
	-for name in $(correct_dir); \
	do \
		cd $$name; touch perf_est.json; cd -; \
		cd $$name; diff --brief perf_est.json ref_perf_est.json >> err.log; cd -; \
		if [ -s "$$name/err.log" ]; then \
			echo "$$name fail" >> err.log; \
			cat $$name/err.log >> err.log; \
		fi \
	done
	-for name in $(incorrect_dir); \
	do \
		if [ -s "$$name/output_vec_add_kernel.json" ]; then \
			echo "$$name fail" >> err.log; \
			cat $$name/err.log >> err.log; \
		fi \
	done

clean:
	-rm -rf *log; 
	-rm -rf *.json; 
	-for name in $(correct_dir); \
	do \
		cd $$name; rm -rf bak.json; cd -; \
		cd $$name; rm -rf step*.json; cd -; \
		cd $$name; rm -rf *require*.json; cd -; \
		cd $$name; rm -rf *output*.json; cd -; \
		cd $$name; rm -rf rose*; cd -; \
		cd $$name; rm -rf history*; cd -; \
		cd $$name; rm -rf kernel_list.json; cd -; \
		cd $$name; rm -rf kernel_file.json; cd -; \
		cd $$name; rm -rf altera_const.h; cd -; \
		cd $$name; rm -rf merlin_type_define.h; cd -; \
		cd $$name; rm -rf run.sh; cd -; \
		cd $$name; rm -rf gen_token.json; cd -; \
		cd $$name; rm -rf host lc*; cd -; \
		cd $$name; rm -rf directive.xml; cd -; \
		cd $$name; rm -rf *log; cd -; \
		cd $$name; rm -rf perf_est.json; cd -; \
	done
	-for name in $(incorrect_dir); \
	do \
		cd $$name; rm -rf bak.json; cd -; \
		cd $$name; rm -rf step*.json; cd -; \
		cd $$name; rm -rf *require*.json; cd -; \
		cd $$name; rm -rf *output*.json; cd -; \
		cd $$name; rm -rf rose*; cd -; \
		cd $$name; rm -rf history*; cd -; \
		cd $$name; rm -rf kernel_list.json; cd -; \
		cd $$name; rm -rf kernel_file.json; cd -; \
		cd $$name; rm -rf altera_const.h; cd -; \
		cd $$name; rm -rf merlin_type_define.h; cd -; \
		cd $$name; rm -rf run.sh; cd -; \
		cd $$name; rm -rf gen_token.json; cd -; \
		cd $$name; rm -rf host lc*; cd -; \
		cd $$name; rm -rf directive.xml; cd -; \
		cd $$name; rm -rf *log; cd -; \
		cd $$name; rm -rf perf_est.json; cd -; \
	done
