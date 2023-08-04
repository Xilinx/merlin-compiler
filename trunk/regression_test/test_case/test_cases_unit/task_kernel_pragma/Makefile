
correct_dir=lab0 lab1 lab2 lab3 lab4

unit_test: diff
run:
	-rm -rf err.log
	-touch err.log
	-for name in `echo $(correct_dir)`; \
	do \
		cd $$name; merlin_create_project run -s src; merlin_flow run; cd -; \
	done

diff:run 
	-for name in $(correct_dir); \
	do \
		if [ ! -s "$$name/run/pkg/libkernel.so" ]; then \
			echo "$$name failed to generate library" >> err.log; \
		fi \
	done
