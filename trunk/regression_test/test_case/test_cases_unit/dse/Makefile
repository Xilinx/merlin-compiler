test_dir= test_ds_parallel \
		  test_ds_pipeline \
		  test_ds_tiling \
		  test_ds_bitwidth \
		  test_ds_skeleton_extract \
		  test_ds_resource_eval 

#test_dse_flow // disabled in 2019-05-16 as suggested by Cod
#test_dse_eval_hls
#test_ds_builder 
#test_ds_profiler 
#test_dse_config_apply 
#test_dse_midend_checker 
gen_ref:
	@rm -rf err.log
	@touch err.log
	@for case in $(test_dir); do \
		make -C $$case clean update; \
	done

unit_test:
	@rm -rf err.log
	@for case in $(test_dir); do \
		make -C $$case clean &> /dev/null; \
		make -C $$case diff &>> dse_unit_test.log; \
		if [ $$? -eq 0 ]; then \
			if [ `stat -c %s $$case/err.log` -eq 0 ]; \
			then echo "$$case...OK"; \
			else \
				echo "$$case...Failed, stop the rest testing" ; \
				echo -e "ERROR: $$case\n `cat $$case/err.log`\n=====" &>> err.log; \
				cat dse_unit_test.log >> err.log; \
				break; \
			fi; \
		else \
			echo "$$case...Failed, stop the rest testing" ; \
			echo -e "ERROR: $$case\n Execution failure\n=====" &>> err.log; \
			cat dse_unit_test.log >> err.log; \
			break; \
		fi; \
	done
	@touch err.log

clean:
	@for case in $(test_dir); do \
		make -C $$case clean &> /dev/null; \
	done
	@rm -f *.log
	@find . -name "err.log"  -prune -exec rm -rf {} \;
	@find . -name ".*finish*"  -prune -exec rm -rf {} \;
