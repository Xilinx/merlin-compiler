correct_dir=vec_add_parallel \
			vec_add_pipeline_both \
			vec_add_pipeline_interface \
			vec_add_pipeline_no_interface \
			vec_add_pipeline_partial_function 
unit_test: diff 
run:
	-rm -rf err.log
	-touch err.log
	-for name in `echo $(correct_dir)`; \
	do \
		cd $$name/build; make clean; make mcc_estimate; cd -; \
	done

update: 
	-for name in $(correct_dir); \
	do \
		echo $$name;	\
		cd $$name/build; cp merlin.rpt golden_merlin.rpt; cd -; \
	done
	
diff: run
	-rm -rf err.log; 
	-touch err.log;
	-for name in $(correct_dir); \
	do \
		echo $$name; \
		cd $$name/build; touch err.log; perl ../../compare.pl; cd -;	\
		if [ -s ./$$name/build/err.log ] ; then \
			echo $$name >> err.log; \
			cat $$name/build/err.log >> err.log;	\
		fi	\
	done

clean:
	find . -name "a.out" -exec rm -rf {} \;
	find . -name "__merlinvec_add_kernel.h" -exec rm -rf {} \;
	find . -name "*.log" -exec rm -rf {} \;
	find . -name "*.Mer" -exec rm -rf {} \;
	find . -name "*.merlin_prj" -exec rm -rf {} \;
	find . -name "*.mco" -exec rm -rf {} \;
	find . -name "*.so" -exec rm -rf {} \;
	find . -name "*.mon" -exec rm -rf {} \;
	find . -name "rose*" -exec rm -rf {} \;
	find . -name ".log*" -exec rm -rf {} \;
	find . -name "log" -exec rm -rf {} \;
	find . -name "test" -exec rm -rf {} \;
	find . -name "*.log" -exec rm -rf {} \;
	find . -name "host.list" -exec rm -rf {} \;
	find . -name "kernel.list" -exec rm -rf {} \;
	find . -name "merlin.rpt" -exec rm -rf {} \;
	find . -name "*.json" -exec rm -rf {} \;
	find . -name "flex*" -exec rm -rf {} \;
	find . -name "kernel_die.h" -exec rm -rf {} \;
	find . -name "__merlin_kernel_list.h" -exec rm -rf {} \;
	find . -name "critical_message.rpt.tt" -exec rm -rf {} \;
	find . -name "__merlin_api_list.c" -exec rm -rf {} \;
	find . -name ".finish_read_files" -exec rm -rf {} \;
	find . -name "__merlin_opencl*" -exec rm -rf {} \;
	find . -name "__merlin_parameter*" -exec rm -rf {} \;
	find . -name "ref___merlinkernel*" -exec rm -rf {} \;
	rm -rf err.log
#		cd $$name; for one in `ls rose_*.c*`; do diff -du  $$one  ref_$$one >>err.log; done; cd -; \
