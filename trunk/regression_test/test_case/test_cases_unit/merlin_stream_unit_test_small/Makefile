correct_dir=pipeline	\
			parallel
incorrect_dir=local_arg	\
			  before_mult_call	\
			  not_before_call	\
			  have_return	\
			  stream_fail	\
			  parallel_factor_fail	\
			  parallel_not_in_loop	\
			  parallel_pragma_position

gen_ref: update
unit_test: diff 
run: clean
	-rm -rf err.log
	-touch err.log
	-for name in `echo $(correct_dir)`; \
	do \
		echo "Running $$name"; \
		cp directive.xml run $$name;	\
		cd $$name; sh run > log; cd -; \
	done
	-for name in `echo $(incorrect_dir)`; \
	do \
		echo "Running $$name"; \
		cp directive.xml run $$name;	\
		cd $$name; sh run > log; cd -; \
	done

update: run
	-for name in $(correct_dir); \
	do \
		echo $$name;	\
		cd $$name; for one in `ls rose_*.c*`; do cp  $$one  ref_$$one; done; cd -; \
		cd $$name; for one in `ls __merlin_parameter* `; do cp  $$one  ref_$$one; done; cd -; \
		cd $$name; for one in `ls __merlin_opencl_kernel_buffer*`; do cp  $$one  ref_$$one; done; cd -; \
		cd $$name; for one in `ls rose___merlinwrapper_*`; do cp  $$one  ref_$$one; done; cd -; \
		cd $$name; for one in `ls rose___merlinkernel* `; do cp  $$one  ref_$$one; done; cd -; \
	done
	
diff: run
	-rm -rf err.log; 
	-touch err.log;
	-for name in $(correct_dir); \
	do \
		echo $$name; \
		cd $$name; for one in `ls rose_*.c*`; do diff -b $$one ref_$$one >> err.log; done; cd -; \
		cd $$name; for one in `ls __merlin_parameter* `; do diff -b $$one  ref_$$one >> err.log; done; cd -; \
		cd $$name; for one in `ls __merlin_opencl_kernel_buffer*`; do diff -b $$one  ref_$$one >> err.log; done; cd -; \
		cd $$name; for one in `ls rose___merlinwrapper_*`; do diff -b $$one ref_$$one >> err.log; done; cd -; \
		cd $$name; for one in `ls rose___merlinkernel* `; do diff -b $$one ref_$$one >> err.log; done; cd -; \
		if [ -s ./$$name/err.log ] ; then \
			echo $$name >> err.log; \
			cat $$name/err.log >> err.log; \
		fi	\
	done
	-for name in $(incorrect_dir); \
	do \
	 	if [ ! -f "$$name/critical_message.rpt" ]; then	\
			echo "$$name error, file not exist" >> $$name/err.log; \
			cat $$name/err.log >> err.log; \
	   	fi;	\
		if [ 0 -eq `grep -c "ERROR" $$name/critical_message.rpt` ]; then \
			echo "$$name error" >> $$name/err.log; \
			cat $$name/err.log >> err.log; \
		fi \
	done

clean:
	find . -name "channel_def.h" -exec rm -rf {} \;
	find . -name "altera_const.cl" -exec rm -rf {} \;
	find . -name "rose*" -exec rm -rf {} \;
	find . -name "log" -exec rm -rf {} \;
	find . -name "*.log" -exec rm -rf {} \;
	find . -name "host.list" -exec rm -rf {} \;
	find . -name "kernel.list" -exec rm -rf {} \;
	find . -name "*.rpt" -exec rm -rf {} \;
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
	find . -name "gdb" -exec rm -rf {} \;
	find . -name "history*" -exec rm -rf {} \;
	find . -name "__merlin_api_list.cpp" -exec rm -rf {} \;
	find . -name "aux" -exec rm -rf {} \;
	rm -rf err.log
#		cd $$name; for one in `ls rose_*.c*`; do diff -du  $$one  ref_$$one >>err.log; done; cd -; \
