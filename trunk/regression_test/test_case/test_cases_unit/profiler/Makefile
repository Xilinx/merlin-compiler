unit_test:
	rm -rf err.log
	touch err.log
	-cd case1/intel/; make clean; make syncheck;
	-grep "ERROR" case1/intel/merlin.log && echo fails >> err.log
	-cd case1/xilinx/; make clean; make syncheck;
	-grep "ERROR" case1/xilinx/merlin.log && echo fails >> err.log
	-cd case2/intel/; make clean; make syncheck;
	-grep "ERROR" case2/intel/merlin.log || echo fails >> err.log
	-cd case2/xilinx/; make clean; make syncheck;
	-grep "ERROR" case2/xilinx/merlin.log || echo fails >> err.log
