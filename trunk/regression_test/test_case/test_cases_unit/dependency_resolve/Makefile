unit_test: 
	make run
	make compare
$(eval cases = $(shell ls case-*.cpp))
# $(eval refs = $(shell ls run-case-*.cpp_rose_case-*.cpp.ref))
refs = $(addprefix run-, $(addsuffix _rose_*.cpp.ref, $(cases)))
logs = $(addprefix run-, $(addsuffix _algo-log.ref, $(cases)))

init: $(addprefix init-, $(cases))
	-touch err.log

run: $(addprefix run-, $(cases))
	-mars_opt recursive_func.cpp -e c -p dependency_resolve 
	-mars_opt no_kernel.cpp  -e c -p dependency_resolve
compare: $(addprefix cmp-log-, $(logs)) $(addprefix cmp-ref-, $(refs))

update: $(addprefix up-log-, $(logs)) $(addprefix up-ref-, $(refs))

init-%: clean
	-mkdir run-$*
	-cp Makefile.tmp run-$*/Makefile
	-cp $* run-$*

run-%: init
	-make -C run-$*

cmp-ref-%: run
	-$(eval cmp_ref_target = $(subst cpp_,cpp/,$(*:%.ref=%)))
	-gcc -c $(cmp_ref_target) 2>> err.log
	-diff -q $* $(cmp_ref_target) &>> err.log

cmp-log-%: run
	-$(eval cmp_log_target = $(subst .cpp_algo-log,.cpp/algo-log,$(*:%.ref=%)))
	-diff -q $* $(cmp_log_target) &>> err.log

up-ref-%:
	-$(eval cmp_ref_target = $(subst cpp_,cpp/,$(*:%.ref=%)))
	-cp $(cmp_ref_target) $*

up-log-%:
	-$(eval cmp_log_target = $(subst .cpp_algo-log,.cpp/algo-log,$(*:%.ref=%)))
	-cp $(cmp_log_target) $*

clean:
	-rm flex*.log
	-rm -rf run-*.cpp
	-rm *.o err.log
