correct_dir=case1 case3 case4 case5
incorrect_dir=case2

unit_test:
	rm -rf err.log
	touch err.log
	-for dir in `echo $(correct_dir)`; \
	do \
		cd $$dir; \
		mars_opt `find *.cpp *.c` -e c -p detect_multidim_pointer -a impl_tool=sdaccel; \
		grep -i "ERROR" critical_message.rpt && echo $$dir fails >> ../../err.log; \
		cd -; \
	done
	-for dir in `echo $(incorrect_dir)`; \
	do \
		cd $$dir; \
		mars_opt `find *.cpp *.c` -e c -p detect_multidim_pointer -a impl_tool=sdaccel; \
		grep -i "ERROR" critical_message.rpt || echo $$dir fails >> ../../err.log; \
		cd -; \
	done
  
clean:
	-for dir in `ls -d */`; \
	do \
		cd $$dir; \
		rm *.log; rm rose*; rm *.rpt; rm *.json; cd -; \
	done
