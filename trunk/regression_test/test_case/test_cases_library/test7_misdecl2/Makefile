test:
	rm -rf err.log
	touch err.log
	merlincc -c *.c -device=aocl::a10gx_es3
	if [ -d "merlincc_export" ]; then echo "pass"; else echo "test7 fail_opencl" >> err.log; fi
