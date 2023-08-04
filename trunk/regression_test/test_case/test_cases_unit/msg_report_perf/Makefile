correct_dir= basic test_case_fully
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
	find . -name "rose*" -exec rm -rf {} \;
	find . -name "log" -exec rm -rf {} \;
	find . -name "flex*" -exec rm -rf {} \;
	find . -name "perf_est.json" -exec rm -rf {} \;
	find . -name "err.log" -exec rm -rf {} \;
	find . -name ".merlin_prj" -exec rm -rf {} \;
	find . -name ".finish_read_files" -exec rm -rf {} \;
	find . -name "first.json" -exec rm -rf {} \;
	find . -name "history*" -exec rm -rf {} \;
	rm -rf err.log;
	-for name in $(correct_dir); \
	do \
		cd $$name; make clean; cd -; \
	done
