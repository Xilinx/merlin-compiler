#correct_dir= test6 test7
#correct_dir= test7
incorrect_dir=test1 test2 test3 test4 test5
gen_ref: update
unit_test: diff 
diff: $(patsubst %,diff-%,$(correct_dir))  $(patsubst %,diff-f-%,$(incorrect_dir))
	cat $(patsubst %,%/err.log,$(correct_dir)) $(patsubst %,%/err.log,$(incorrect_dir)) > err.log
run-%: 
	$(eval $@_dir := $(patsubst run-%,%,$@))
	-make mcc_runsim -C $($@_dir)

diff-%: run-%
	$(eval $@_dir := $(patsubst diff-%,%,$@))
	rm -rf $($@_dir)/err.log
	touch $($@_dir)/err.log
		if [ ! -f "$($@_dir)/pass.o" ]; then \
			echo "$($@_dir)/fail" >> $($@_dir)/err.log; \
		fi 

diff-f-%: run-%
	$(eval $@_dir := $(patsubst diff-f-%,%,$@))
	rm -rf $($@_dir)/err.log
	touch $($@_dir)/err.log
		if grep \"ERROR\" $($@_dir)/merlin.log; then \
			echo "$($@_dir) should report error" >> $($@_dir)/err.log; \
		fi 

clean:
	-find . -name "pass.o*" -exec rm -rf {} \;
	-find . -name "fail.o" -exec rm -rf {} \;
	-find . -name "*.log" -exec rm -rf {} \;
	-find . -name "*.rpt" -exec rm -rf {} \;
	-find . -name "test_*" -exec rm -rf {} \;
	-find . -name "emconfig.json" -exec rm -rf {} \;
	-find . -name "*.h" -exec rm -rf {} \;
	-find . -name "pass.o" -exec rm -rf {} \;
	-find . -name "sdaccel*" -exec rm -rf {} \;
	-find . -name "lib*" -exec rm -rf {} \;
	-for name in $(correct_dir); \
	do \
		cd $$name; rm -rf .merlin_prj; cd -; \
	done
	-for name in $(incorrect_dir); \
	do \
		cd $$name; rm -rf .merlin_prj; cd -; \
	done
	-rm -rf err.log
