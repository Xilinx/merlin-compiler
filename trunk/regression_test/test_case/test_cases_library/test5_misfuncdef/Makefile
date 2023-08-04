VARIABLE = ""
test:
	rm -rf err.log
	touch err.log
	merlincc -c *.c -device=aocl::a10gx_es3 >> log
	merlincc merlincc_export/ -flow=simrun >> log
	perl check.pl
