test:
	rm -rf err.log
	touch err.log
	#medium default standard
	rm -rf run
	merlin_create_project run -s src -keep_platform
	if [ -f "run/spec/directive.xml" -a -f "run/run.prj" ]; then echo "pass"; else echo "fail to create project" >> err.log; fi
	if [ 0 -ne `grep -c "standard" run/spec/directive.xml` ]; then echo "pass"; else echo "failed to create standard project" >> err.log; fi
	if [ 0 -ne `grep -c "aocl" run/spec/directive.xml` ]; then echo "pass"; else echo "failed to create aocl project" >> err.log; fi
	merlin_flow run
	if [ -d "run/implement/export" -a -f run/report/merlin.log ]; then echo "pass"; else echo "fail to generate medium export" >> err.log; fi
	merlin_flow run host
	if [ -d "run/implement/exec/host" -a -f run/report/merlin.log -a -f "run/pkg/host_top" -a -f "run/pkg/libkernel.so" ]; then echo "pass"; else echo "fail to generate medium host" >> err.log; fi
	merlin_flow run hls
	if [ -d "run/implement/exec/hls" -a -f "run/report/merlin.log" ]; then echo "pass"; else echo "fail to run medium hls" >> err.log; fi
	merlin_flow run sim --attribute host_args="1234"
	if [ -d "run/implement/exec/run_sim" -a -f "run/report/merlin.log"  -a -f "run/implement/exec/run_sim/pass.o" ]; then echo "pass"; else echo "fail to run medium sim" >> err.log; fi
	merlin_flow run test -no_bit_test --attribute host_args="1234"
	if [ 0 -ne `grep -c "OpenCL Sim on CPU    : Pass" run/implement/merlin_summary.rpt` ]; then echo "pass"; else echo "fail to run medium test 1" >> err.log; fi
	#baseline
#	rm -rf run
#	merlin_create_project run -s src -keep_platform
#	perl -pi -e 's/<directives>/<directives>\n<pure_kernel>off<\/pure_kernel>/g' run/spec/directive.xml
#	perl -pi -e 's/<pure_kernel>.*?</<pure_kernel>off</g' run/spec/directive.xml
#	merlin_flow run
#	if [ -d "run/implement/export" -a -f run/report/opt/merlin_opt.log -a -d run/report/.source_opt  ]; then echo "pass"; else echo "fail to generate baseline export" >> err.log; fi
#	merlin_flow run host
#	if [ -d "run/implement/exec/host" -a -f run/report/host/merlin_host.log -a -f "run/pkg/host_top" ]; then echo "pass"; else echo "fail to generate baseline host" >> err.log; fi
#	merlin_flow run hls
#	if [ -d "run/implement/exec/hls" -a -f "run/report/hls/resource_detail.rpt" -a -f "run/report/hls/merlin_hls.log" ]; then echo "pass"; else echo "fail to run baseline hls" >> err.log; fi
#	merlin_flow run sim --args "1234"
#	if [ -d "run/implement/exec/run_sim" -a -f "run/report/sim/merlin_sim.log"  -a -f "run/implement/exec/run_sim/bin/pass.o" ]; then echo "pass"; else echo "fail to run baseline sim" >> err.log; fi
#	merlin_flow run test -no_bit_test --args "1234"
#	if [ 0 -ne `grep -c "OpenCL Sim on CPU    : Pass" run/implement/merlin_summary.rpt` ]; then echo "pass"; else echo "fail to run baseline test" >> err.log; fi

#	#low
#	rm -rf run
#	merlin_create_project run -s src -keep_platform
#	perl -pi -e 's/<opt_effort>.*?</<opt_effort>low</g' run/spec/directive.xml
#	merlin_flow run
#	if [ -d "run/implement/export" -a -f run/report/opt/merlin_opt.log -a -d run/report/.source_opt  ]; then echo "pass"; else echo "fail to generate low export" >> err.log; fi
#	merlin_flow run host
#	if [ -d "run/implement/exec/host" -a -f run/report/host/merlin_host.log -a -f "run/pkg/host_top" -a -f "run/pkg/libkernel.so" ]; then echo "pass"; else echo "fail to generate low host" >> err.log; fi
#	merlin_flow run hls
#	if [ -d "run/implement/exec/hls" -a -f "run/report/hls/resource_detail.rpt" -a -f "run/report/hls/merlin_hls.log" ]; then echo "pass"; else echo "fail to run low hls" >> err.log; fi
#	merlin_flow run sim --args "1234"
#	if [ -d "run/implement/exec/run_sim" -a -f "run/report/sim/merlin_sim.log"  -a -f "run/implement/exec/run_sim/bin/pass.o" ]; then echo "pass"; else echo "fail to run low sim" >> err.log; fi
#	merlin_flow run test -no_bit_test --args "1234"
#	if [ 0 -ne `grep -c "OpenCL Sim on CPU    : Pass" run/implement/merlin_summary.rpt` ]; then echo "pass"; else echo "fail to run low test" >> err.log; fi
#	#high
#	rm -rf run
#	merlin_create_project run -s src -keep_platform
#	perl -pi -e 's/<opt_effort>.*?</<opt_effort>high</g' run/spec/directive.xml
#	merlin_flow run
#	if [ -d "run/implement/export" -a -f run/report/opt/merlin_opt.log -a -d run/report/.source_opt  ]; then echo "pass"; else echo "fail to generate high export" >> err.log; fi
#	merlin_flow run host
#	if [ -d "run/implement/exec/host" -a -f run/report/host/merlin_host.log -a -f "run/pkg/host_top" -a -f "run/pkg/libkernel.so" ]; then echo "pass"; else echo "fail to generate high host" >> err.log; fi
#	merlin_flow run hls
#	if [ -d "run/implement/exec/hls" -a -f "run/report/hls/resource_detail.rpt" -a -f "run/report/hls/merlin_hls.log" ]; then echo "pass"; else echo "fail to run high hls" >> err.log; fi
#	merlin_flow run sim --args "1234"
#	if [ -d "run/implement/exec/run_sim" -a -f "run/report/sim/merlin_sim.log"  -a -f "run/implement/exec/run_sim/bin/pass.o" ]; then echo "pass"; else echo "fail to run high sim" >> err.log; fi
#	merlin_flow run test -no_bit_test --args "1234"
#	if [ 0 -ne `grep -c "OpenCL Sim on CPU    : Pass" run/implement/merlin_summary.rpt` ]; then echo "pass"; else echo "fail to run high test" >> err.log; fi
