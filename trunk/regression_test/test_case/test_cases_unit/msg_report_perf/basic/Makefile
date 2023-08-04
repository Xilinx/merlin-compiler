correct_dir=test001 test002 test003 test004 test005 test006 test007 test008 \
			test009 test010 test011 test012 test013 test014 test015 test016 \
			test017 test018 test019 test020 test021 test022 test023 test024 \
			test025 test026 test027 test028 test029 test030 test031 test032	\
			test033 test034 test035 test036 test037 test038 test039 test040	\
			test041 test042 test043 test044 test045 test046 test047 test048 \
			test049 test050 test051 test052 test053 test054 test055 test056	\
			test057 test058 test059 test060 test061 test062 test063 test064	\
			test065 test066 test067 test068 test069 test070 test071 test072	\
			test073 test074 test075 test076

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
		cd $$name; cp ../directive.xml .; cd -; \
		cd $$name; sh run.sh > perf_est.log; cd -; \
	done

update:
	-for name in $(correct_dir); \
	do \
		cd $$name; cp perf_est.json  ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report_perf/basic/$$name/ref.json; cd -; \
		cd $$name; cp perf_est.log   ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report_perf/basic/$$name/ref.log;  cd -; \
	done

# only check cycles of all unit, no others
diff:
	-rm -rf err.log; 
	-touch err.log; 
	-for name in $(correct_dir); \
	do \
		echo "Compare: $$name";	\
		cd $$name; touch perf_est.json; cd -; \
		cd $$name; perl ../compare.pl ref.json perf_est.json ref.log perf_est.log; cd -; \
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
