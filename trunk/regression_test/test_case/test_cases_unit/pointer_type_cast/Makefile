correct_dir=case1 case5 case6 case7 case8
incorrect_dir=case2 case3 case4 case9 

unit_test:
	rm -rf err.log
	touch err.log
	-for dir in `echo $(correct_dir)`; \
	do \
		cd $$dir/src; \
		mars_opt `find *.cpp *.c` -e c -p detect_pointer_type_cast -a impl_tool=sdaccel; \
		grep -i "ERROR" critical_message.rpt && echo $$dir fails >> ../../err.log; \
		cd -; \
	done
	-for dir in `echo $(incorrect_dir)`; \
	do \
		cd $$dir/src; \
		mars_opt `find *.cpp *.c` -e c -p detect_pointer_type_cast -a impl_tool=sdaccel; \
		diff critical_message.rpt critical_message.ref >> ../../err.log; \
		cd -; \
	done
  
update:
	-for dir in `echo $(incorrect_dir)`; \
	do \
		cd $$dir/src; \
		mars_opt `find *.cpp *.c` -e c -p detect_pointer_type_cast -a impl_tool=sdaccel; \
		mv critical_message.rpt critical_message.ref; \
		cd -; \
	done

clean:
	-for dir in `ls -d */`; \
	do \
		cd $$dir/xilinx_build; make clean; rm -rf .Mer; rm -rf .merlin_prj;rm *.log; rm rose*; rm *.rpt; rm *.json; cd -; \
		cd $$dir/src; rm .*; rm *.log; rm rose*; rm *.rpt; rm *.json; cd -; \
	done 
