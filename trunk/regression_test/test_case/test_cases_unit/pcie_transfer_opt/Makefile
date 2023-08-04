correct_dir= vec_add_1d vec_add_2d vec_add_struct_1d vec_add_struct_2d
incorrect_dir=
unit_test: diff 
diff: $(patsubst %,diff-%,$(correct_dir))  $(patsubst %,diff-f-%,$(incorrect_dir))
	cat $(patsubst %,%/build/err.log,$(correct_dir)) $(patsubst %,%/build/err.log,$(incorrect_dir)) > err.log

run-%: 
	$(eval $@_dir := $(patsubst run-%,%,$@))
	-make mcc_runsim -C $($@_dir)/build

diff-%: run-%
	$(eval $@_dir := $(patsubst diff-%,%,$@))
	rm -rf $($@_dir)/build/err.log
	touch $($@_dir)/build/err.log
		if [ ! -f "$($@_dir)/build/pass.o" ]; then \
			echo "$($@_dir)/build/fail" >> $($@_dir)/build/err.log; \
		fi 

diff-f-%: run-%
	$(eval $@_dir := $(patsubst diff-f-%,%,$@))
	rm -rf $($@_dir)/build/err.log
	touch $($@_dir)/build/err.log
		if grep \"ERROR\" $($@_dir)/build/merlin.log; then \
			echo "$($@_dir) should report error" >> $($@_dir)/build/err.log; \
		fi 


clean:
	-find . -name "a.out" -exec rm -rf {} \;
	-find . -name "pass.o*" -exec rm -rf {} \;
	-find . -name "fail.o" -exec rm -rf {} \;
	-find . -name "*.log" -exec rm -rf {} \;
	-find . -name "*.rpt" -exec rm -rf {} \;
	-find . -name "test_*" -exec rm -rf {} \;
	-find . -name "emconfig.json" -exec rm -rf {} \;
	-find . -name "__*.h" -exec rm -rf {} \;
	-find . -name "pass.o" -exec rm -rf {} \;
	-find . -name "sdaccel*" -exec rm -rf {} \;
	-find . -name "lib*" -exec rm -rf {} \;
	-find . -name "*.mon" -exec rm -rf {} \;
	-find . -name "*.mco" -exec rm -rf {} \;
	-find . -name "*.xclbin" -exec rm -rf {} \;
	-find . -name ".merlin_prj" -exec rm -rf {} \;
	-find . -name ".Mer" -exec rm -rf {} \;
	-find . -name ".run" -exec rm -rf {} \;
	-find . -name "*.pl" -exec rm -rf {} \;
	-rm -rf err.log
