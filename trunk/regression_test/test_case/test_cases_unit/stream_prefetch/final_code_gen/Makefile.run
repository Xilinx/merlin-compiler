test:
	- bash run &> /dev/null
	- diff -du rose___merlinkernel_vec_add.cpp rose___merlinkernel_vec_add.cpp.ref &>> ../err.log
	- diff -du rose___merlinwrapper_vec_add.cpp rose___merlinwrapper_vec_add.cpp.ref &>> ../err.log
update:
	- bash run &> /dev/null
	- cp rose___merlinkernel_vec_add.cpp rose___merlinkernel_vec_add.cpp.ref 
	- cp rose___merlinwrapper_vec_add.cpp rose___merlinwrapper_vec_add.cpp.ref 
clean:
	- rm *.log
	- rm *.rpt
	- rm *.list
	- rm *.json
	- rm .finish_*
	- rm rose_succeed
	- rm rose_*.cpp
	- rm __merlin_kernel_list.h
	- rm channel_def.h
