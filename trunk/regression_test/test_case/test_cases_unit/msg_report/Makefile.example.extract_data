REPORT=run/implement/exec/hls/report_merlin/msg_report.out
REPORT_PATH=run/implement/exec/hls/report_merlin/

test:
	-rm -rf err.log
	-rm -rf run
	-touch err.log
	-tar zxf run.tgz
	python $(MERLIN_COMPILER_HOME)/mars-gen/scripts/msg_report/msg_report.py report_dir 1 || echo 'extract data fail' >> err.log
	cp report_dir/gen_token.json . || echo 'no gen_token.json' >> err.log
	python $(MERLIN_COMPILER_HOME)/regression_test/test_case/test_cases_unit/msg_report/json_compare.py gen_token.json gen_token.json.ref

fast_update:
	python $(MERLIN_COMPILER_HOME)/mars-gen/scripts/msg_report/msg_report.py report_dir 1
	cp report_dir/gen_token.json gen_token.json.ref

update:
	python $(MERLIN_COMPILER_HOME)/mars-gen/scripts/msg_report/msg_report.py report_dir 1
	cp report_dir/gen_token.json gen_token.json.ref
	tar zcf run.tgz report_dir
