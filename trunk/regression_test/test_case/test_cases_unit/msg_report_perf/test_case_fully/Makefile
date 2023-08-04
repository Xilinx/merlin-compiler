correct_dir=test001 test002 test003

incorrect_dir=
gen_ref: clean run update
unit_test: clean run diff 
run:
	-rm -rf err.log
	-touch err.log
	-for name in `echo $(correct_dir)`; \
	do \
		echo "Run case: $$name";	\
		cd $$name; cp ../../common/* lc; cd -; \
		cd $$name; cp ../../common/run.sh .; cd -; \
		cd $$name; sh run.sh > perf_est.log; cd -; \
	done

update:
	-for name in $(correct_dir); \
	do \
		cd $$name; cp perf_est.json  ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report_perf/test_case_fully/$$name/ref.json; cd -; \
		cd $$name; cp perf_est.log   ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report_perf/test_case_fully/$$name/ref.log;  cd -; \
	done

# only check cycles of all unit, no others
diff:
	-rm -rf err.log; 
	-touch err.log; 
	-for name in $(correct_dir); \
	do \
		echo "Compare: $$name";	\
		cd $$name; touch perf_est.json; cd -; \
		cd $$name; perl ../../common/compare.pl ref.json perf_est.json ref.log perf_est.log; cd -; \
		cd $$name; touch err.log; cd -; \
		if [ -s "$$name/err.log" ]; then \
			echo "$$name fail" >> err.log; \
			cat $$name/err.log >> err.log; \
		fi \
	done

clean:
	-find test* -name "compare.pl" -exec rm -rf {} \;
	-find . -name "lc_sim" -exec rm -rf {} \;
	-find . -name "lc_dse" -exec rm -rf {} \;
	-find . -name "lc_rose" -exec rm -rf {} \;
	-find . -name "err.log" -exec rm -rf {} \;
	-find . -name "host" -exec rm -rf {} \;
	-find . -name "__merlin_kernel_list.h" -exec rm -rf {} \;
	-find . -name "log" -exec rm -rf {} \;
	-find . -name "rose*" -exec rm -rf {} \;
	-find . -name "flex*" -exec rm -rf {} \;
	-find . -name "perf_est.json" -exec rm -rf {} \;
	-find . -name "perf_est.log" -exec rm -rf {} \;
	-find . -name "err.log" -exec rm -rf {} \;
	-find . -name ".merlin_prj" -exec rm -rf {} \;
	-find . -name ".finish_read_files" -exec rm -rf {} \;
	-find . -name "first.json" -exec rm -rf {} \;
	-find . -name "history*" -exec rm -rf {} \;
	-find . -name ".Mer" -exec rm -rf {} \;
	-find . -name "bak.json" -exec rm -rf {} \;
	-find . -name "aaa.json" -exec rm -rf {} \;
	-find . -name "lohg" -exec rm -rf {} \;
	-find . -name "memcpy_*.h" -exec rm -rf {} \;
	-find . -name "mars_wide_bus*.h" -exec rm -rf {} \;
	-find . -name "run.sh" -exec rm -rf {} \;
	-rm -rf err.log perf_est.log ref.json
