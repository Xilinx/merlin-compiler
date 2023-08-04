REPORT=run/implement/exec/hls/report_merlin/data.json

test:
	-rm -rf err.log
	-rm -rf run
	-touch err.log
	-tar zxf run.tgz 
	python -O $(MERLIN_COMPILER_HOME)/mars-gen/scripts/msg_report/msg_report.py run all run/implement/exec/hls/report_merlin
	python ../../../numerical_compare.py $(REPORT) data.json.ref

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
	cp run/implement/exec/hls/report_merlin/data.json data.json.ref
	tar zcf run.tgz run
