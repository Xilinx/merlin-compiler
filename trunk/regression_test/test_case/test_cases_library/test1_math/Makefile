test:
	rm -rf err.log
	touch err.log
	merlincc -c *.c -device=aocl::a10gx_es3 -O1
	if [ -d "merlincc_export" ]; then echo "pass"; else echo "test1 fail_opencl" >> err.log; fi
	rm -rf log
	merlincc merlincc_export/ -flow=simrun >> log
	perl check.pl
