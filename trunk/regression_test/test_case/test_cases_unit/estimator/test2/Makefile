REPORT=run/implement/exec/hls/report_merlin/msg_report.out
REPORT_PATH=run/implement/exec/hls/report_merlin/

.PHONY: test
.PHONY: update
.PHONY: fast_update

test:
	-rm -rf *.log *.json code_transform_base directive.xml implement ori_src report
	-touch err.log
	-tar zxf run.tgz 
	python -O $(MERLIN_COMPILER_HOME)/mars-gen/scripts/merlin_estimator/est_top.py --cfg_id 4 --run_dir . --trans_dir code_transform_base --src ori_src --metadata ori_src/metadata --dst ori_src --xml directive.xml --impl_tool aocl --debug 0 --hls_time_limit 100
	diff -du cache_ref/res/1.json cache/res/1.json&>> err.log
	diff -du cache_ref/perf/1.json cache/perf/1.json&>> err.log

update:
	rm -rf *.log *.json code_transform_base directive.xml implement ori_src report
	tar zxf run.tgz 
	python -O $(MERLIN_COMPILER_HOME)/mars-gen/scripts/merlin_estimator/est_top.py --cfg_id 4 --run_dir . --trans_dir code_transform_base --src ori_src --metadata ori_src/metadata --dst ori_src --xml directive.xml --impl_tool aocl --debug 0 --hls_time_limit 100
	cp -rf cache cache_ref
	tar zcf run.tgz code_transform_base implement ori_src cache directive.xml
	rm -rf *.log *.json code_transform_base directive.xml implement ori_src report cache spec
