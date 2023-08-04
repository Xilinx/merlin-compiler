correct_dir=test3_kernel_kernel 
incorrect_dir=test4_task_position test5_kernel_position 
incorrect_dir+= test8_repeat_task test10_repeat_task
incorrect_dir+= test11_no_kernel test12_kernel_name test12_kernel_name_2
incorrect_dir+= test13_repeat_kernel
#test9_repeat_task 
#test6_task_position test7_repeat_task 
all_test:   build unit_test
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

update:run 
	-for name in $(correct_dir); \
	do \
		cp -f $$name/rose_vec_add.cpp $$name/ref_vec_add.cpp; \
		cp -f $$name/rose_vec_add_kernel.cpp $$name/ref_vec_add_kernel.cpp; \
	done
	
diff:run 
	-for name in $(correct_dir); \
	do \
		diff --brief $$name/rose_vec_add.cpp $$name/ref_vec_add.cpp &>> err.log; \
		diff --brief $$name/rose_vec_add_kernel.cpp $$name/ref_vec_add_kernel.cpp &>> err.log; \
	done
	-for name in $(incorrect_dir); \
	do \
		grep -R "PRAGM" $$name/critical_message.rpt >> $$name/err.log; \
		if [ ! -s "$$name/err.log" ]; then \
			echo "$$name should report error" &>> err.log; \
		fi \
	done

clean:
	find . -name "rose*" -exec rm -rf {} \;
	find . -name "flex*" -exec rm -rf {} \;
	find . -name "*.log" -exec rm -rf {} \;
	find . -name "*.rpt" -exec rm -rf {} \;
	find . -name ".finish_read_files" -exec rm -rf {} \;
	find . -name "history_kernel_pragma_gen.json" -exec rm -rf {} \;
	rm -rf err.log
