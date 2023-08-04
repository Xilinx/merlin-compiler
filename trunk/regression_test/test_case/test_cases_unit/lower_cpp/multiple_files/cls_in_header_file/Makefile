unit_test: run compare

run: clean
	-touch err.log
	-mars_opt *.cpp -e c -p lower_separate

$(eval kernel_files_ref=$(shell ls __merlinkernel_*.cpp.ref))
kernel_files := $(kernel_files_ref:%.cpp.ref=%.cpp)

compare-%: run
	-gcc -c $* 2>> err.log
	-diff $* $*.ref &>> err.log

update-%:
	-cp $* $*.ref

compare: $(addprefix compare-, $(kernel_files))

update: run $(addprefix update-, $(kernel_files))

clean:
	-rm -rf *.list __merlin_kernel_list.h merlin_type_define.h
	-rm -rf rose_*.cpp __merlinkernel_*.cpp
	-rm -rf *.log *.json rose_succeed
	-rm -rf *.o
	-rm -rf err.log
