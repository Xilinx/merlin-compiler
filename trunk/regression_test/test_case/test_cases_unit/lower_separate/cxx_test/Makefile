unit_test: compare

$(eval cases = $(shell ls case-*.cpp))
$(eval refs = $(shell ls *cpp_*merlin*.ref))
$(eval msgs = $(shell ls *critical_message.rpt.ref))

init: $(addprefix init-, $(cases))
	-touch err.log

run: $(addprefix run-, $(cases)) run-special-multiple.cpp

run-special-multiple.cpp: clean
	-mkdir $@
	-cp Makefile.tmp $@/Makefile
	-cp case-multiple-kernels.cpp case-cls.cpp common.h common.cpp $@
	-make -C $@

compare: $(addprefix cmp-msg-, $(msgs)) $(addprefix cmp-ref-, $(refs))

update: $(addprefix up-msg-, $(msgs)) $(addprefix up-ref-, $(refs))

init-%: clean
	-mkdir run-$*
	-cp Makefile.tmp run-$*/Makefile
	-cp $* common.h common1.h common.cpp run-$*

run-%: init
	-make -C run-$*

cmp-ref-%: run
	-$(eval cmp_ref_target = $(subst cpp_,cpp/,$(*:%.ref=%)))
	-gcc -c $(cmp_ref_target) 2>> err.log
	-diff -q $* $(cmp_ref_target) &>> err.log

cmp-msg-%: run
	-$(eval cmp_msg_target = $(subst .cpp_critical_message.rpt,.cpp/critical_message.rpt,$(*:%.rpt.ref=%.rpt)))
	-diff -q $* $(cmp_msg_target) &>> err.log

up-ref-%:
	-$(eval cmp_ref_target = $(subst cpp_,cpp/,$(*:%.ref=%)))
	-cp $(cmp_ref_target) $*

up-msg-%:
	-$(eval cmp_msg_target = $(subst .cpp_critical_message.rpt,.cpp/critical_message.rpt,$(*:%.rpt.ref=%.rpt)))
	-cp $(cmp_msg_target) $*

gen_ref: update
clean:
	-rm -rf flex*.log
	-rm -rf run-*.cpp
	-rm -rf *.o err.log

.PHONY: run-special-multiple.cpp
