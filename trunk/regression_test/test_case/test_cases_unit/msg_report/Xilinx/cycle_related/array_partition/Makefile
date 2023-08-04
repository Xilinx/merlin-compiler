REPORT=run/implement/exec/hls/report_merlin/msg_report.out
REPORT_PATH=run/implement/exec/hls/report_merlin/

.PHONY: test
.PHONY: update
.PHONY: fast_update

test:
	-rm -rf err.log
	-rm -rf run
	-touch err.log
	-tar zxf run.tgz 
	python -O $(MERLIN_COMPILER_HOME)/mars-gen/scripts/msg_report/msg_report.py run all run/implement/exec/hls/report_merlin
	sed -i 's/.*Report time.*//' $(REPORT)
	cp $(REPORT) ./msg_report.out
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/extract_data_from_report.py ./msg_report.out
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/extract_data_from_report.py ./msg_report.out.ref
	python $(MERLIN_COMPILER_HOME)/regression_test/test_case/test_cases_unit/msg_report/numerical_compare.py msg_report.out.data msg_report.out.ref.data
	diff -du msg_report.out.cleaned msg_report.out.ref.cleaned &>> err.log

fast_update:
	python -O $(MERLIN_COMPILER_HOME)/mars-gen/scripts/msg_report/msg_report.py run all run/implement/exec/hls/report_merlin
	cp ${REPORT} .
	sed -i 's/.*Report time.*//' msg_report.out
	cp ${REPORT_PATH}/gen_token.json gen_token.json
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/json_postprocess.py gen_token.json
	cp ${REPORT_PATH}/perf_est.json perf_est.json
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/json_postprocess.py perf_est.json
	cp ${REPORT_PATH}/refer_induct.json refer_induct.json
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/json_postprocess.py refer_induct.json
	cp ${REPORT_PATH}/final_info.json final_info.json
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/json_postprocess.py final_info.json

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
	cp $(REPORT) ./msg_report.out.ref
	sed -i 's/.*Report time.*//' msg_report.out.ref
	tar zcf run.tgz run
