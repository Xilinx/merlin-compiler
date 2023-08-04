correct_dir= basic
incorrect_dir=
gen_ref: update
unit_test: diff 
run: clean
	-rm -rf err.log
	-touch err.log
	-for name in $(correct_dir); \
	do \
		cd $$name; make run; cd -; \
	done

update: run
	-for name in $(correct_dir); \
	do \
		cd $$name; make update; cd -; \
	done
	
diff: run
	-for name in $(correct_dir); \
	do \
		cd $$name; make diff; cd -; \
		if [ -s "$$name/err.log" ]; then \
			echo "$$name fail" >> err.log; \
			cat $$name/err.log >> err.log; \
		fi \
	done

clean:
	rm -rf err.log;
	-for name in $(correct_dir); \
	do \
		cd $$name; make clean; cd -; \
	done
