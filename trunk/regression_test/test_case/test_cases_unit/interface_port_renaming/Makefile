correct_dir=test_basic test_complex_literal test_hls 

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
		else \
		  cp -f $$name/rose_vec_add.c $$name/ref_rose_vec_add.c; \
		fi \
	done

diff: run
	-for name in $(correct_dir); \
	do \
		if [ -e $$name/rose_vec_add.cpp ]; then \
		  diff --brief $$name/rose_vec_add.cpp $$name/ref_rose_vec_add.cpp &>> err.log; \
		else \
		  diff --brief $$name/rose_vec_add.c $$name/ref_rose_vec_add.c &>> err.log; \
		fi \
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
