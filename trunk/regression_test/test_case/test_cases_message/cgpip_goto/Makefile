include common.mk
REPORT=run/implement/code_transform/critical_message.rpt
test:
	rm -rf run err.log
	touch err.log
	-merlin_create_project run -s src -xilinx
	sed -i '/loop_tiling/i <stream_prefetch> off </stream_prefetch>' run/spec/directive.xml
	-merlin_flow run
	if [ -f run/cmost_flow_has_error ]; then echo "fails">> err.log; fi;
	-diff run/implement/code_transform/critical_message.rpt critical_message.ref >> err.log


update: 
	-cp ${REPORT} ${MERLIN_COMPILER_HOME}/regression_test/test_case/test_cases_message/cgpip_goto/critical_message.ref
