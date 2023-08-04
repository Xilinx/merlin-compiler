auto_fg_cases = auto_fg_test.cpp test_imperfect.cpp test_tripcount.cpp test_prior.cpp dwt3d_kernel.c logistic_regression_kernel.c vec_add_kernel_lab3.c \
					gradient.cpp normal_kernel.cpp namd_kernel.c twonn.cpp test_auto_val.c test_hls.cpp auto_fg_test_2.cpp auto_fg_with_hls_unroll.cpp \
					auto_fg_cgpar.cpp
unit_test: diff
run-auto-fg-%: %
	-rm -rf $^.err.log
	-touch $^.err.log
	-mars_opt $^ -e c -p midend_preprocess -a impl_tool=sdaccel 2>$^.tmp.log || (cat $^.tmp.log >> $^.err.log)
	-mars_opt rose_$^ -e c -p auto_parallel -a impl_tool=sdaccel 2>$^.tmp.log || (cat $^.tmp.log >> $^.err.log)
	-mv rose_rose_$^ rose_$^
run:  
	-rm -rf run_large_const; mkdir run_large_const; cd run_large_const; mars_opt ../large_const/__merlinkernel_svm_top.cpp -a impl_tool=aocl\
		-e c -p midend_preprocess 2>tmp.log || (cat tmp.log >> err.log)
update: $(patsubst %,update-auto-fg-%, $(auto_fg_cases))
	-cp run_large_const/critical_message.rpt test_rule/critical_message_large_const.ref 
diff: run $(patsubst %,diff-auto-fg-%, $(auto_fg_cases)) 
	cat $(patsubst %,%.err.log, $(auto_fg_cases)) > err.log	
	-diff --brief run_large_const/critical_message.rpt test_rule/critical_message_large_const.ref &>> err.log
diff-auto-fg-%: run-auto-fg-%
	$(eval $_case := $(patsubst diff-auto-fg-%,%,$@))
	-base_name=`basename $($_case) .c`; \
	base_name=`basename $${base_name} .cpp`; \
	diff --brief rose_$($_case) test_rule/auto_fg/$${base_name}.out.ref &>> $($_case).err.log
update-auto-fg-%: 
	$(eval $_case := $(patsubst update-auto-fg-%,%,$@))
	-base_name=`basename $($_case) .c`; \
	base_name=`basename $${base_name} .cpp`; \
	cp rose_$($_case) test_rule/auto_fg/$${base_name}.out.ref
