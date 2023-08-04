correct_dir=memory_burst_4d memory_burst_4d_multi_level memory_burst_single multi_call_duplicate \
			unroll_loop_has_arg missing_id with_dram_param merlin_memcpy_x

incorrect_dir=
gen_ref: update
unit_test: diff 
run: clean
	-rm -rf err.log
	-touch err.log
	-for name in `echo $(correct_dir)`; \
	do \
		rm -rf $$name; \
	done
	-tar zxvf run.tar
	-for name in `echo $(correct_dir)`; \
	do \
		cd $$name; sh run.sh; cd -; \
	done

update: run
	-for name in $(correct_dir); \
	do \
		cd $$name; cp perf_est.json  ref_perf_est.json; cd -; \
	done
	
diff: run
	-for name in $(correct_dir); \
	do \
		cd $$name; touch perf_est.json; cd -; \
		cd $$name; diff --brief perf_est.json ref_perf_est.json >> err.log; cd -; \
		if [ -s "$$name/err.log" ]; then \
			echo "$$name fail" >> err.log; \
			cat $$name/err.log >> err.log; \
		fi \
	done

clean:
	find . -name "host" -exec rm -rf {} \;
	find . -name "lc_rose" -exec rm -rf {} \;
	find . -name "lc_sim" -exec rm -rf {} \;
	find . -name "rose*" -exec rm -rf {} \;
	find . -name "*log" -exec rm -rf {} \;
	find . -name "flex*" -exec rm -rf {} \;
	find . -name "perf_est.json" -exec rm -rf {} \;
	find . -name "err.log" -exec rm -rf {} \;
	find . -name ".merlin_prj" -exec rm -rf {} \;
	find . -name ".finish_read_files" -exec rm -rf {} \;
	find . -name "first.json" -exec rm -rf {} \;
	find . -name "history*" -exec rm -rf {} \;
	rm -rf err.log
