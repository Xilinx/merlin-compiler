REPORT=run/implement/exec/hls/report_merlin/msg_report.out
REPORT_PATH=run/implement/exec/hls/report_merlin/

.PHONY: test
.PHONY: update

test:
	-rm -rf err.log
	-rm -rf run
	-touch err.log
	-tar zxf run.tgz 
	python -O $(MERLIN_COMPILER_HOME)/mars-gen/scripts/msg_report/msg_report.py run all run/implement/exec/hls/report_merlin
	cp $(REPORT) .
	cp $(REPORT_PATH)/*.json .
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/extract_data_from_report.py ./msg_report.out
	diff -du merlin.rpt.cleaned cleaned.ref &>> err.log
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/json_compare.py gen_token.json gen_token.json.ref
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/json_compare.py perf_est.json perf_est.json.ref
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/json_compare.py refer_induct.json refer_induct.json.ref
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/json_compare.py final_info.json final_info.json.ref

update:
	merlin_flow run -s
	merlin_flow run hls
	find run/implement/exec/hls/ -maxdepth 1 -mindepth 1|grep -v report_merlin|xargs rm -rf
	rm -rf run/implement/code_transform
	rm -rf run/implement/aux
	rm -rf run/implement/dse
	rm -rf run/implement/opencl
	rm -rf run/implement/src_opt
	rm -rf run/implement/src_org
	rm -rf run/implement/exec/hls/report_merlin/final_report
	rm -rf run/implement/exec/hls/report_merlin/ref_induct
	rm -rf run/implement/exec/hls/report_merlin/perf_est
	rm -rf run/implement/exec/hls/report_merlin/gen_token
	rm -rf run/pkg
	cp ${REPORT} .
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/extract_data_from_report.py ./msg_report.out
	cp merlin.rpt.cleaned cleaned.ref 
	cp ${REPORT_PATH}/gen_token.json gen_token.json.ref
	cp ${REPORT_PATH}/perf_est.json perf_est.json.ref
	cp ${REPORT_PATH}/refer_induct.json refer_induct.json.ref
	cp ${REPORT_PATH}/final_info.json final_info.json.ref
	tar zcf run.tgz run
