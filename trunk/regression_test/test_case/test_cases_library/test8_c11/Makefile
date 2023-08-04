test:
	rm -rf err.log
	touch err.log
	merlincc -c *.c -device=aocl::a10gx_es3 -std=c1x
	if [ -d "merlincc_export" ]; then echo "pass"; else echo "test8 fail_opencl" >> err.log; fi
