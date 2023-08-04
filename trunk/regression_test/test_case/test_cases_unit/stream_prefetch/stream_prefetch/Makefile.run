test:
	-rm -f critical_message.rpt;
	- bash run &> /dev/null
	- diff -du rose___merlinkernel_vec_add.cpp rose___merlinkernel_vec_add.cpp.ref &>> ../err.log
	- gcc -fsyntax-only rose___merlinkernel_vec_add.cpp -I ../ 2>tmp.log || cat tmp.log >> ../err.log
	- diff -du rose___merlinwrapper_vec_add.cpp rose___merlinwrapper_vec_add.cpp.ref &>> ../err.log
	-if [ -f critical_message.rpt.ref ]; then \
	  diff -du critical_message.rpt critical_message.rpt.ref &>> ../err.log; \
	  fi

update:
	- bash run &> /dev/null
	- cp rose___merlinwrapper_vec_add.cpp rose___merlinwrapper_vec_add.cpp.ref
	- cp rose___merlinkernel_vec_add.cpp rose___merlinkernel_vec_add.cpp.ref 
	-if [ -f critical_message.rpt ]; then \
	  cp critical_message.rpt critical_message.rpt.ref; \
	  fi
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
