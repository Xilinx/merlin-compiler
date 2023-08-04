test:
	export XILINX_SDX=aaa
	rm -rf err.log
	touch err.log
	rm -rf log1 log2 log3 log4 log5 log6 log7
	rm -rf run1 run2 run3 run4 run5 run6 run7
	export ALTERA_QUARTUS_HOME=aaa; merlin_create_project run1 -s src -keep_platform > log1
	if [ 0 -ne `grep -c "failed" log1` ]; then echo "pass"; else echo "failed to detect run1 environment" >> err.log; fi
	export ALTERAOCLSDKROOT=aaa; merlin_create_project run2 -s src -keep_platform > log2
	if [ 0 -ne `grep -c "failed" log2` ]; then echo "pass"; else echo "failed to detect run2 environment" >> err.log; fi
	export AOCL_BOARD_PACKAGE_ROOT=aaa; merlin_create_project run3 -s src -keep_platform > log3
	if [ 0 -ne `grep -c "failed" log3` ]; then echo "pass"; else echo "failed to detect run3 environment" >> err.log; fi
	export ALTERA_QUARTUS_HOME=aaa; merlin_create_project run4 -s src -a -keep_platform > log4
	if [ 0 -ne `grep -c "failed" log4` ]; then echo "pass"; else echo "failed to detect run4 environment" >> err.log; fi
	export ALTERAOCLSDKROOT=aaa; merlin_create_project run5 -s src -a -keep_platform > log5
	if [ 0 -ne `grep -c "failed" log5` ]; then echo "pass"; else echo "failed to detect run5 environment" >> err.log; fi
	export AOCL_BOARD_PACKAGE_ROOT=aaa; merlin_create_project run6 -s src -a -keep_platform > log6
	if [ 0 -ne `grep -c "failed" log6` ]; then echo "pass"; else echo "failed to detect run6 environment" >> err.log; fi
	export XILINX_SDX=aaa; merlin_create_project run7 -s src -x -keep_platform > log7
	if [ 0 -ne `grep -c "failed" log7` ]; then echo "pass"; else echo "failed to detect run7 environment" >> err.log; fi
