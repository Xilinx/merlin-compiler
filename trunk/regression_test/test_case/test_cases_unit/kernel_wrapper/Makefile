correct_dir=all_api copy_directive die extern rename_arg \
			static_decl test_global_array_null_index const_decl test_global_char_array
incorrect_dir=no_decl
gen_ref: update
unit_test: diff 
run: clean
	-rm -rf err.log
	-touch err.log
	-for name in `echo $(correct_dir)`; \
	do \
		cd $$name; sh run.sh; cd -; \
	done
	-for name in `echo $(incorrect_dir)`; \
	do \
		cd $$name; sh run.sh; cd -; \
	done

update: run
	-for name in $(correct_dir); \
	do \
		cd $$name; for one in `ls __merlinwrapper_*.c*`; do mv  $$one  ref_$$one; done; cd -; \
		cd $$name; for one in `ls __merlinhead_kernel_top.c* `; do mv  $$one  ref_$$one; done; cd -; \
		cd $$name; for one in `ls rose_*.c*`; do mv  $$one  ref_$$one; done; cd -; \
		cd $$name; for one in `ls kernel_die.h`; do mv  $$one  ref_$$one; done; cd -; \
	done
	
diff: run
	-for name in $(correct_dir); \
	do \
		cd $$name; for one in `ls __merlinwrapper_*.c*`; do diff --brief  $$one  ref_$$one >>err.log; done; cd -; \
		cd $$name; for one in `ls __merlinhead_kernel_top.c* `; do diff --brief  $$one  ref_$$one >>err.log; done; cd -; \
		cd $$name; for one in `ls rose_*.c*`; do diff --brief  $$one  ref_$$one >>err.log; done; cd -; \
		cd $$name; for one in `ls kernel_die.h`; do diff --brief  $$one  ref_$$one >>err.log; done; cd -; \
	 	cat $$name/err.log >> err.log; \
	done
	-for name in $(incorrect_dir); \
	do \
		if [ -s "$$name/rose.log" ]; then \
			echo "$$name should report error" >> $$name/err.log; \
	 		cat $$name/err.log >> err.log; \
		fi \
	done

clean:
	find . -name "rose*" -exec rm -rf {} \;
	find . -name "*.log" -exec rm -rf {} \;
	find . -name "host.list" -exec rm -rf {} \;
	find . -name "kernel.list" -exec rm -rf {} \;
	find . -name "*.rpt" -exec rm -rf {} \;
	find . -name "*.json" -exec rm -rf {} \;
	find . -name "flex*" -exec rm -rf {} \;
	find . -name "__merlinwrapper*" -exec rm -rf {} \;
	find . -name "kernel_die.h" -exec rm -rf {} \;
	find . -name ".finish_read_files" -exec rm -rf {} \;
	find . -name "__merlin_api_list.c" -exec rm -rf {} \;
	rm -rf err.log
