correct_dir=case1 case2 

unit_test: diff
run: 
	rm -rf err.log
	touch err.log
	-for dir in `echo $(correct_dir)`; \
	do \
		cd $$dir/src; \
		rm -rf critical_message.rpt rose* *.json; \
		mars_opt `find *.c*` -e c -p detect_pointer_compare -a impl_tool=sdaccel; \
		grep -i "ERROR" critical_message.rpt || echo $$dir fails >> ../../err.log; \
		cd -; \
	done
  
clean:
	-for dir in `ls -d */`; \
	do \
		cd $$dir/src; rm .*; rm *.log; rm rose*; rm *.rpt; rm *.json; rm *.ref; cd -; \
	done 

gen_ref: update

update: run
	-for dir in `echo $(correct_dir)`; \
	do \
		cd $$dir/src; \
		mv critical_message.rpt critical_message.ref; \
		cd -; \
	done

diff: run
	-for dir in `echo $(correct_dir)`; \
	do \
		cd $$dir/src; \
		diff -du critical_message.rpt critical_message.ref &>> ../../err.log; \
		cd -; \
	done



