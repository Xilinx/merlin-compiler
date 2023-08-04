correct_dir=test_basic test_func 

gen_ref: update
unit_test: diff
run: $(patsubst %, run-%, $(correct_dir)) 

update: $(patsubst %, update-%, $(correct_dir)) 
run-%:
	$(eval $@_dir := $(patsubst run-%,%,$@))
	-cd $($@_dir) && sh run.sh;
update-%: run-%
	$(eval $@_dir := $(patsubst update-%,%,$@))
	-if [ -e $($@_dir)/rose_vec_add.cpp ]; then \
	  cp -f $($@_dir)/rose_vec_add.cpp $($@_dir)/vec_add.cpp.ref; \
	else \
	  cp -f $($@_dir)/rose_vec_add.c $($@_dir)/vec_add.c.ref; \
	fi \
	

diff: $(patsubst %, diff-%, $(correct_dir)) 
	cat $(patsubst %, %.err.log, $(correct_dir)) > err.log
diff-%: run-%
	$(eval $@_dir := $(patsubst diff-%,%,$@))
	rm -rf $($@_dir).err.log
		-if [ -e $($@_dir)/rose_vec_add.cpp ]; then \
		  diff --brief $($@_dir)/rose_vec_add.cpp $($@_dir)/vec_add.cpp.ref &>> $($@_dir).err.log; \
		else \
		  diff --brief $($@_dir)/rose_vec_add.c $($@_dir)/vec_add.c.ref &>> $($@_dir).err.log; \
		fi 

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
