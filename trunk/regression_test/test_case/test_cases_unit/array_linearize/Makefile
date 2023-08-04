all_test:   build unit_test
unit_test: diff

supported_cases = test1.c \
	test2.c \
	test3.c \
	test4.c \
	multi_dim_array_success.cpp \
  test_standarise_pntr.cpp \
	write_interface_success.c \
	test_ref_in_index.c

unsupported_cases = multi_dim_array_fail.cpp \
	write_interface_fail.c \
	test_f0_write.c \
  test_invalid_standarise_pntr.cpp \
	test_a_fail.c \
	test_array_dim.c \
	test_multi_array_preprocess.c \
	test_conflict_array_dim.c \
  test0.c \
  test_runingtime.c

hls_cases = test_hls.c
additional_preprocess_supported_cases = add_test1.c \
	add_test2.c \
	add_test3.c \
	add_test4.c \
	add_test5.c \
	add_test6.c \
	add_test7.c \
	add_test8.c \
	add_test9.c \
	add_test11.c \
	add_test13.c

additional_linearize_supported_cases = add_test5.c \
	add_test7.c \
	add_test8.c \
	add_test13.c 

additional_preprocess_unsupported_cases = add_test0.c \
	add_test10.c \
	add_test12.c \
	add_test14.c 

additional_linearize_unsupported_cases = add_test0.c \
	add_test1.c \
  add_test2.c \
	add_test3.c \
	add_test6.c \
	add_test4.c \
	add_test9.c \
	add_test10.c \
	add_test11.c \
	add_test12.c \
	add_test14.c 

run: $(patsubst %,run-s-%,${supported_cases}) \
	$(patsubst %,run-us-%,${unsupported_cases}) \
	$(patsubst %,run-hls-%,${hls_cases}) \
    $(patsubst %,run-aps-%,${additional_preprocess_supported_cases}) \
    $(patsubst %,run-apus-%,${additional_preprocess_unsupported_cases}) \
    $(patsubst %,run-als-%,${additional_linearize_supported_cases}) \
    $(patsubst %,run-alus-%,${additional_linearize_unsupported_cases}) 
run-s-%: 
	$(eval $@f := $(patsubst run-s-%,%,$@))
	rm -rf ${$@f}.err.log; touch ${$@f}.err.log
	-mars_opt ${$@f} -e c -p array_linearize -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> ${$@f}.err.log); \
	  gcc -c -std=c99 -fsyntax-only rose_${$@f} 2>tmp.log || (cat tmp.log >> ${$@f}.err.log)
run-hls-%: 
	$(eval $@f := $(patsubst run-hls-%,%,$@))
	rm -rf ${$@f}.err.log; touch ${$@f}.err.log
	-mars_opt ${$@f} -e c -p array_linearize -a impl_tool=sdaccel -a tool_version=vivado_hls 2>tmp.log || (cat tmp.log >> ${$@f}.err.log); \
	  ucc -c -std=c99 -fsyntax-only rose_${$@f} 2>tmp.log || (cat tmp.log >> ${$@f}.err.log)
run-us-%:
	$(eval $@f := $(patsubst run-us-%,%,$@))
	-rm -rf run-${$@f}; mkdir run-${$@f}; cd run-${$@f}; touch critical_message.rpt; mars_opt ../${$@f} -e c -p array_linearize -a impl_tool=sdaccel
run-aps-%:
	$(eval $@f := $(patsubst run-aps-%,%,$@))
	-rm -rf run_aps_${$@f}; mkdir run_aps_${$@f}; cd run_aps_${$@f};  mars_opt ../${$@f} -e c -p synthesis_check -a impl_tool=sdaccel -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log); \
	  mars_opt rose_${$@f} -I ../ -e c -p preprocess -a impl_tool=sdaccel -a pure_kernel=on 2>tmp.log || (cat tmp.log >> err.log); \
	  gcc -c -std=c99 -fsyntax-only -I ../ rose_rose_${$@f} 2>tmp.log || (cat tmp.log >> err.log); \
	  mv rose_rose_${$@f} rose_preprocess_${$@f}
run-als-%:
	$(eval $@f := $(patsubst run-als-%,%,$@))
	 -rm -rf run_als_${$@f}; mkdir run_als_${$@f}; cd run_als_${$@f};   mars_opt ../${$@f} -e c -p array_linearize -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log); \
	  gcc -c -std=c99 -fsyntax-only -I ../ rose_${$@f} 2>tmp.log || (cat tmp.log >> err.log); \
	  mv rose_${$@f} rose_linearize_${$@f}
run-apus-%:
	$(eval $@f := $(patsubst run-apus-%,%,$@))
	-rm -rf run_apus_${$@f}; mkdir run_apus_${$@f}; cd run_apus_${$@f};  touch critical_message.rpt; mars_opt ../${$@f} -e c -p preprocess -a impl_tool=sdaccel -a pure_kernel=on
run-alus-%:	
	$(eval $@f := $(patsubst run-alus-%,%,$@))
	 -rm -rf run_alus_${$@f}; mkdir run_alus_${$@f}; cd run_alus_${$@f}; touch critical_message.rpt;  mars_opt ../${$@f} -e c -p array_linearize -a impl_tool=sdaccel


#   -mars_opt test_local2.cpp -e c -p struct_decompose 2>tmp.log || (cat tmp.log >> err.log)
#	-mars_opt test_stl_member.cpp -e c -p struct_decompose 2>tmp.log || (cat tmp.log >> err.log)


diff: $(patsubst %,diff-s-%,${supported_cases}) \
	$(patsubst %,diff-us-%,${unsupported_cases}) \
	$(patsubst %,diff-hls-%,${hls_cases}) \
    $(patsubst %,diff-aps-%,${additional_preprocess_supported_cases}) \
    $(patsubst %,diff-apus-%,${additional_preprocess_unsupported_cases}) \
    $(patsubst %,diff-als-%,${additional_linearize_supported_cases}) \
    $(patsubst %,diff-alus-%,${additional_linearize_unsupported_cases})
	cat $(patsubst %,%.err.log,${supported_cases}) \
	$(patsubst %,run-%/err.log,${unsupported_cases}) \
    $(patsubst %,run_aps_%/err.log,${additional_preprocess_supported_cases}) \
    $(patsubst %,run_apus_%/err.log,${additional_preprocess_unsupported_cases}) \
    $(patsubst %,run_als_%/err.log,${additional_linearize_supported_cases}) \
    $(patsubst %,run_alus_%/err.log,${additional_linearize_unsupported_cases}) >err.log
diff-s-%: run-s-%
	$(eval $@f := $(patsubst diff-s-%,%,$@))
	-diff --brief rose_${$@f} test_rule/rose_${$@f} >> ${$@f}.err.log
	
diff-hls-%: run-hls-%
	$(eval $@f := $(patsubst diff-hls-%,%,$@))
	-diff --brief rose_${$@f} test_rule/rose_${$@f} >> ${$@f}.err.log
	
diff-us-%: run-us-%
	$(eval $@f := $(patsubst diff-us-%,%,$@))
	-cd run-${$@f}; diff --brief critical_message.rpt ../test_rule/${$@f}.rpt &>> err.log
	
diff-aps-%:run-aps-%
	$(eval $@f := $(patsubst diff-aps-%,%,$@))
	-cd run_aps_${$@f}; diff --brief rose_preprocess_${$@f} ../test_rule/rose_preprocess_${$@f} &>> err.log
diff-als-%:run-als-%
	$(eval $@f := $(patsubst diff-als-%,%,$@))
	-cd run_als_${$@f}; diff --brief rose_linearize_${$@f} ../test_rule/rose_linearize_${$@f} &>> err.log
diff-apus-%:run-apus-%
	$(eval $@f := $(patsubst diff-apus-%,%,$@))
	-cd run_apus_${$@f}; diff --brief critical_message.rpt ../test_rule/${$@f}.apus.rpt &>> err.log
diff-alus-%:run-alus-%
	$(eval $@f := $(patsubst diff-alus-%,%,$@))
	-cd run_alus_${$@f}; diff --brief critical_message.rpt ../test_rule/${$@f}.alus.rpt &>> err.log

update: $(patsubst %,update-s-%,${supported_cases}) \
	$(patsubst %,update-us-%,${unsupported_cases}) \
	$(patsubst %,update-hls-%,${hls_cases}) \
    $(patsubst %,update-aps-%,${additional_preprocess_supported_cases}) \
    $(patsubst %,update-apus-%,${additional_preprocess_unsupported_cases}) \
    $(patsubst %,update-als-%,${additional_linearize_supported_cases}) \
    $(patsubst %,update-alus-%,${additional_linearize_unsupported_cases}) 
update-s-%: run-s-%
	$(eval $@f := $(patsubst update-s-%,%,$@))
	cp rose_${$@f} test_rule/rose_${$@f} 
	
update-hls-%: run-hls-%
	$(eval $@f := $(patsubst update-hls-%,%,$@))
	cp rose_${$@f} test_rule/rose_${$@f} 
	
update-us-%: run-us-%
	$(eval $@f := $(patsubst update-us-%,%,$@))
	cp run-${$@f}/critical_message.rpt test_rule/${$@f}.rpt 
	
update-aps-%:run-aps-%
	$(eval $@f := $(patsubst update-aps-%,%,$@))
	cp run_aps_${$@f}/rose_preprocess_${$@f} test_rule/rose_preprocess_${$@f} 
update-als-%:run-als-%
	$(eval $@f := $(patsubst update-als-%,%,$@))
	cp 	run_als_${$@f}/rose_linearize_${$@f} test_rule/rose_linearize_${$@f} 
update-apus-%:run-apus-%
	$(eval $@f := $(patsubst update-apus-%,%,$@))
	cp run_apus_${$@f}/critical_message.rpt test_rule/${$@f}.apus.rpt 
update-alus-%:run-alus-%
	$(eval $@f := $(patsubst update-alus-%,%,$@))
	cp run_alus_${$@f}/critical_message.rpt test_rule/${$@f}.alus.rpt 


clean:
	rm -rf rose* *.o
