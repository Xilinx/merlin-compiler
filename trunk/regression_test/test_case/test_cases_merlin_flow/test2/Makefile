test:
	rm -rf err.log
	touch err.log
#	#medium
	rm -rf run
	merlin_create_project run -s src -x -keep_platform
	if [ -f "run/spec/directive.xml" -a -f "run/run.prj" ]; then echo "pass"; else echo "fail to create project" >> err.log; fi
	if [ 0 -ne `grep -c "sdaccel" run/spec/directive.xml` ]; then echo "pass"; else echo "failed to create sdaccel project" >> err.log; fi
	merlin_flow run
	if [ -d "run/implement/export" -a -f run/report/merlin.log ]; then echo "pass"; else echo "fail to generate export" >> err.log; fi
	merlin_flow run host
	if [ -d "run/implement/exec/host" -a -f run/report/merlin.log -a -f "run/pkg/host_top" -a -f "run/pkg/libkernel.so" ]; then echo "pass"; else echo "fail to generate host" >> err.log; fi
	merlin_flow run hls
	if [ -d "run/implement/exec/hls" -a -f "run/report/merlin.rpt" ]; then echo "pass"; else echo "fail to run hls" >> err.log; fi
	merlin_flow run sim --attribute host_args="1234"
	if [ -d "run/implement/exec/run_sim" -a -f "run/report/merlin.log"  -a -f "run/implement/exec/run_sim/pass.o" ]; then echo "pass"; else echo "fail to run sim" >> err.log; fi
	merlin_flow run test -no_bit_test --attribute host_args="1234"
	if [ 0 -ne `grep -c "OpenCL Sim on CPU    : Pass" run/implement/merlin_summary.rpt` ]; then echo "pass"; else echo "fail to run test" >> err.log; fi
	#baseline
#	rm -rf run
#	merlin_create_project run -s src -x -keep_platform
#	perl -pi -e 's/<naive_hls>.*?</<naive_hls>on</g' run/spec/directive.xml
#	perl -pi -e 's/<directives>/<directives>\n<pure_kernel>off<\/pure_kernel>/g' run/spec/directive.xml
#	merlin_flow run
#	if [ -d "run/implement/export" -a -f run/report/opt/merlin_opt.log -a -d run/report/.source_opt  ]; then echo "pass"; else echo "fail to generate export" >> err.log; fi
#	merlin_flow run host
#	if [ -d "run/implement/exec/host" -a -f run/report/host/merlin_host.log -a -f "run/pkg/host_top" ]; then echo "pass"; else echo "fail to generate naive host" >> err.log; fi
#	merlin_flow run hls
#	if [ -d "run/implement/exec/hls" -a -f "run/report/hls/system_estimate.xtxt" -a -f "run/report/hls/merlin_hls.log" ]; then echo "pass"; else echo "fail to run naive hls" >> err.log; fi
#	merlin_flow run sim --args "1234"
#	if [ -d "run/implement/exec/run_sim" -a -f "run/report/sim/merlin_sim.log"  -a -f "run/implement/exec/run_sim/pass.o" ]; then echo "pass"; else echo "fail to run naive sim" >> err.log; fi
#	merlin_flow run test -no_bit_test --args "1234"
#	if [ 0 -ne `grep -c "OpenCL Sim on CPU    : Pass" run/implement/merlin_summary.rpt` ]; then echo "pass"; else echo "fail to run naive test" >> err.log; fi
