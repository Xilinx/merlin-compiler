correct_dir=2d_strstr_intint 2d_intint_nonnon 2d_intint_intint 2d_nonnon_nonnon 2d_nonnon_intint 
correct_dir+=2d_strint_intint 
correct_dir+=1dp_str_int 1dp_int_int 1dp_int_non
correct_dir+=1da_str_int 1da_int_int 1da_int_non
incorrect_dir=2d_strstr_strstr 2d_strstr_nonnon 2d_intint_strstr 2d_nonnon_strstr 
incorrect_dir+=2d_intint_strint 2d_expint_intint 2d_intint_expint
incorrect_dir+=1dp_non_non 1dp_non_int 1dp_non_str 1dp_str_str 1dp_str_non 1dp_int_str 1dp_strexp_int 1da_int_-int 1da_str_float
full_dir=src_1daa_str_int src_1dap_str_int src_1dpa_str_int src_1dpp_str_int 
full_dir+=src_2daa_strstr_intint src_2dah_strstr_intint src_2daa_strstr_intint_overflow src_2dhh_strstr_intint  src_2dpp_strstr_intint
full_error_dir=src_2dph_strstr_intint src_1dpp_str_int_position 

all_test:   build unit_test
gen_ref: run update
unit_test: run diff 
run:
	-rm -rf err.log
	-touch err.log
	-for name in `echo $(correct_dir)`; \
	do \
		cd exec/$$name; sh run.sh; cd -; \
	done
	-for name in `echo $(incorrect_dir)`; \
	do \
		cd exec/$$name; sh run.sh; cd -; \
	done
	-for name in `echo $(full_dir)`; \
	do \
		cd full; merlin_create_project run_$$name -s $$name; merlin_flow run_$$name sim -s --args "100 10"; cd -; \
	done
	-for name in `echo $(full_error_dir)`; \
	do \
		cd full; merlin_create_project run_$$name -s $$name; merlin_flow run_$$name sim -s --args "100 10"; cd -; \
	done

update:
	-for name in $(correct_dir); \
	do \
		cp -f exec/$$name/rose_* ref/$$name; \
	done
	
diff:
	-for name in $(correct_dir); \
	do \
		diff --brief exec/$$name/rose___merlinwrapper_test_kernel.c ref/$$name/rose___merlinwrapper_test_kernel.c &>> err.log; \
	done
	-for name in $(incorrect_dir); \
	do \
		grep -R "INFTF-3" exec/$$name/critical_message.rpt >> exec/$$name/err.log; \
		grep -R "MDARR-3" exec/$$name/critical_message.rpt >> exec/$$name/err.log; \
		if [ ! -s "exec/$$name/err.log" ]; then \
			echo "exec/$$name should report error" &>> err.log; \
		fi \
	done
	-for name in `echo $(full_dir)`; \
	do \
		if [ ! -f "full/run_$$name/implement/exec/run_sim/bin/pass.o" ]; then   \
			echo "full/run_$$name/implement/exec/run_sim/bin/pass.o failed" &>> err.log; \
		fi \
	done
	-for name in `echo $(full_error_dir)`; \
	do \
		if [ -f "full/run_$$name/implement/exec/run_sim/bin/pass.o" ] && [ -d "full/run_$$name/implement/export" ]; then   \
			echo "full/run_$$name/implement/exec/run_sim/bin/pass.o should failed" &>> err.log; \
		fi \
	done

clean:
	find  exec -name "rose*" -exec rm -rf {} \;
	find  exec -name "flex*" -exec rm -rf {} \;
	find  exec -name "*.log" -exec rm -rf {} \;
	find  exec -name "*.rpt" -exec rm -rf {} \;
	find  full -name "run*" -exec rm -rf {} \;
	find  full -name ".merlin.finish" -exec rm -rf {} \;
	rm -rf err.log
