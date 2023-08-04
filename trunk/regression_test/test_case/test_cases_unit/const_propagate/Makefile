correct_dir=test_basic test_types test_calls test_pointer test_multidef \
			test_loops test_scopes test_template_const_variable test_pragmas \
			test_enum_in_pragma

incorrect_dir= 
                

gen_ref: update
unit_test: diff
unit_test_multi: diff_multi
run: clean
	-rm -rf err.log
	-touch err.log
	-for name in `echo $(correct_dir)`; \
	do \
		cd $$name; sh run.sh; cd -; \
	done

update: run
	-for name in $(correct_dir); \
	do \
		if [ -e $$name/rose_vec_add.cpp ]; then \
		  cp -f $$name/rose_vec_add.cpp $$name/vec_add.cpp.ref; \
		else \
		  cp -f $$name/rose_vec_add.c $$name/vec_add.c.ref; \
		fi \
	done

diff: run
	-for name in $(correct_dir); \
	do \
		if [ -e $$name/rose_vec_add.cpp ]; then \
		  diff --brief $$name/rose_vec_add.cpp $$name/vec_add.cpp.ref &>> err.log; \
		else \
		  diff --brief $$name/rose_vec_add.c $$name/vec_add.c.ref &>> err.log; \
		fi \
	done

diff_multi: run
	-for name in $(correct_dir); \
	do \
		if [ -e $$name/rose_vec_add.cpp ]; then \
		  diff --brief $$name/rose_vec_add.cpp $$name/vec_add_multi.cpp.ref &>> err.log; \
		else \
		  diff --brief $$name/rose_vec_add.c $$name/vec_add_multi.c.ref &>> err.log; \
		fi \
	done


clean:
	find . -name "rose*" -exec rm -rf {} \;
	find . -name "*.log" -exec rm -rf {} \;
	find . -name "host.list" -exec rm -rf {} \;
	find . -name "kernel.list" -exec rm -rf {} \;
	find . -name "*.rpt" -exec rm -rf {} \;
	find . -name "*.json" -exec rm -rf {} \;
	find . -name ".finish_read_files" -exec rm -rf {} \;
	find . -name "__merlin*" -exec rm -rf {} \;
	find . -name "merlin*" -exec rm -rf {} \;
	rm -rf err.log
