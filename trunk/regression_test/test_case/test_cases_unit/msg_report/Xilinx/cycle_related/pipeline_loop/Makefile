REPORT=run/implement/exec/hls/report_merlin/msg_report.out

.PHONY: test
.PHONY: update

test:
	-rm -rf err.log
	-rm -rf run
	-touch err.log
	-tar zxf run.tgz 
	python -O $(MERLIN_COMPILER_HOME)/mars-gen/scripts/msg_report/msg_report.py run all run/implement/exec/hls/report_merlin
	cp $(REPORT) ./msg_report.out
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/extract_data_from_report.py ./msg_report.out
	python $(MERLIN_COMPILER_HOME)/regression_test/test_case/test_cases_unit/msg_report/numerical_compare.py merlin.rpt.data data.ref
	diff -du merlin.rpt.cleaned cleaned.ref &>> err.log

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
	cp $(REPORT) ./msg_report.out
	python ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_unit/msg_report/extract_data_from_report.py ./msg_report.out
	cp merlin.rpt.data data.ref
	cp merlin.rpt.cleaned cleaned.ref
	tar zcf run.tgz run
