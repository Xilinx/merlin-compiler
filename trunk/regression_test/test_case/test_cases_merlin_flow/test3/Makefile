test:
	rm -rf err.log
	touch err.log
	rm -rf log
	rm -rf run1
	merlin_create_project run1 -s src1 -keep_platform > log1
	if [ 0 -ne `grep -c "failed" log1` ]; then echo "pass"; else echo "failed to detect run1 unsupported files" >> err.log; fi
	rm -rf run2
	merlin_create_project run2 -s src2 -keep_platform > log2
	if [ 0 -ne `grep -c "failed" log2` ]; then echo "pass"; else echo "failed to detect run2 unsupported files" >> err.log; fi
	rm -rf run3
	merlin_create_project run3 -s src3 -keep_platform > log3
	if [ 0 -ne `grep -c "failed" log3` ]; then echo "pass"; else echo "failed to detect run3 unsupported files" >> err.log; fi
	rm -rf run4
	merlin_create_project run4 -s src4 -keep_platform > log4
	if [ 0 -ne `grep -c "failed" log4` ]; then echo "pass"; else echo "failed to detect run4 unsupported files" >> err.log; fi
	rm -rf run5
	merlin_create_project run5 -s src5 -keep_platform > log5
	if [ 0 -ne `grep -c "failed" log5` ]; then echo "pass"; else echo "failed to detect run5 unsupported files" >> err.log; fi
	rm -rf run6
	merlin_create_project run6 -s src6 -keep_platform > log6
	if [ 0 -ne `grep -c "failed" log6` ]; then echo "pass"; else echo "failed to detect run6 unsupported files" >> err.log; fi
	rm -rf run7
	merlin_create_project run7 -s src7 -keep_platform > log7
	if [ 0 -ne `grep -c "failed" log7` ]; then echo "pass"; else echo "failed to detect run7 unsupported files" >> err.log; fi
	rm -rf run8
	merlin_create_project run8 -s src8 -keep_platform > log8
	if [ 0 -ne `grep -c "failed" log8` ]; then echo "pass"; else echo "failed to detect run8 unsupported files" >> err.log; fi
	rm -rf run9
	merlin_create_project run9 -s src9 -keep_platform > log9
	if [ 0 -ne `grep -c "failed" log9` ]; then echo "pass"; else echo "failed to detect run9 unsupported files" >> err.log; fi
	rm -rf run10
	merlin_create_project run10 -s src10 -keep_platform > log10
	if [ 0 -ne `grep -c "failed" log10` ]; then echo "pass"; else echo "failed to detect run10 unsupported files" >> err.log; fi
	rm -rf run11
	merlin_create_project run11 -s src11 -keep_platform > log11
	if [ 0 -ne `grep -c "failed" log11` ]; then echo "pass"; else echo "failed to detect run11 unsupported files" >> err.log; fi
	merlin_create_project run12 -s src12 -keep_platform > log12
	if [ 0 -ne `grep -c "#aaa" run12/run12.prj` ]; then echo "pass"; else echo "failed to detect run12 unsupported files" >> err.log; fi
	if [ 0 -ne `grep -c "aaa~" run12/run12.prj` ]; then echo "pass"; else echo "failed to detect run12 unsupported files" >> err.log; fi
