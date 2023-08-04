correct_dir=test_basic overflow hls_stream cast_exp ap_int bool_field self_assign 

incorrect_dir= 
                

gen_ref: update
unit_test: diff
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
		  cp -f $$name/rose_vec_add.cpp $$name/ref_rose_vec_add.cpp; \
		fi; \
		if [ -e $$name/rose_vec_add_wrapper.cpp ]; then \
		  cp -f $$name/rose_vec_add_wrapper.cpp $$name/ref_rose_vec_add_wrapper.cpp; \
		fi; \
		if [ -e $$name/rose_vec_add.c ]; then \
		  cp -f $$name/rose_vec_add.c $$name/ref_rose_vec_add.c; \
		fi; \
		if [ -e $$name/rose_vec_add_wrapper.c ]; then \
		  cp -f $$name/rose_vec_add_wrapper.c $$name/ref_rose_vec_add_wrapper.c; \
		fi; \
	done

diff: run
	-for name in $(correct_dir); \
	do \
		if [ -e $$name/rose_vec_add.cpp ]; then \
		  gcc -c -I $$name -fsyntax-only -I $(MERLIN_COMPILER_HOME)/source-opt/include/apint_include $$name/rose_vec_add.cpp 2>tmp.log || cat tmp.log >> err.log; \
		  diff --brief $$name/rose_vec_add.cpp $$name/ref_rose_vec_add.cpp &>> err.log; \
		fi; \
		if [ -e $$name/rose_vec_add_wrapper.cpp ]; then \
		  gcc -c -I $$name -fsyntax-only -I $(MERLIN_COMPILER_HOME)/source-opt/include/apint_include $$name/rose_vec_add_wrapper.cpp 2>tmp.log || cat tmp.log >> err.log; \
		  diff --brief $$name/rose_vec_add_wrapper.cpp $$name/ref_rose_vec_add_wrapper.cpp; \
		fi; \
		if [ -e $$name/rose_vec_add.c ]; then \
		  gcc -c -I $$name -fsyntax-only $$name/rose_vec_add.c 2>tmp.log || cat tmp.log >> err.log; \
		  diff --brief $$name/rose_vec_add.c $$name/ref_rose_vec_add.c &>> err.log; \
		fi; \
		if [ -e $$name/rose_vec_add_wrapper.c ]; then \
		  gcc -c -I $$name -fsyntax-only $$name/rose_vec_add_wrapper.c 2>tmp.log || cat tmp.log >> err.log; \
		  diff --brief $$name/rose_vec_add_wrapper.c $$name/ref_rose_vec_add_wrapper.c; \
		fi; \
	done

clean:
	find . -name "rose*" -exec rm -rf {} \;
	find . -name "*.log" -exec rm -rf {} \;
	find . -name "host.list" -exec rm -rf {} \;
	find . -name "kernel.list" -exec rm -rf {} \;
	find . -name "*.rpt" -exec rm -rf {} \;
	find . -name "*.json" -exec rm -rf {} \;
	find . -name "__merlin*" -exec rm -rf {} \;
	find . -name "merlin*" -exec rm -rf {} \;
	rm -rf err.log
