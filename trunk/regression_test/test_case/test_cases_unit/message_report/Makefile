#dir = $(abspath $(lastword $(MAKEFILE_LIST)))
dir = $(shell pwd)
usage:
	@echo -e "\nTypical Usage:"
	@echo -e "  make unit_test"
	@echo -e "  make unit_test path=json_parser"
	@echo -e "  make unit_test path=json_parser/test_nested_level.py"
	@echo -e "  make clean\n"

unit_test:
	-rm -rf err.log;
	-touch err.log;
	@python3 -c "import main; main.test('$(path)')"
	-if [ 0 -ne `grep -c "===.*fail" __result__/result.txt` ]; then \
		echo "failed to pass all unit test" >> err.log; \
	fi
	-if [ 1 -ne `grep -c "TOTAL.*100%" __result__/result.txt` ]; then \
		echo "failed to pass pytest coverage" >> err.log; \
	fi
	-rm -rf log_lint
	-cd $(MERLIN_COMPILER_HOME)/mars-gen/scripts/msg_report/xilinx_enhance; PYTHONPATH=. pylint --disable=all --enable=E *.py > $(dir)/log_lint; cd -;
	-if [ 1 -ne `grep -c "rated at 10.00" log_lint` ]; then \
		echo "failed to pass pylint" >> err.log; \
	fi

clean:
	@python3 -c "import main; main.clean()"
	-rm -rf log_lint err.log *.log *.json report_translate/*.log report_fill_source_data/fill_source_data.json report_data_extraction/vivado_hls_bp.json msg_traceback
	-cd misc_modules/report_merlin_fail/; rm -rf gen_token perf_est refer_induct vec_add_kernel final_report *.log *.json
	-cd misc_modules/report_merlin/; rm -rf gen_token perf_est refer_induct vec_add_kernel final_report *.log *.json
