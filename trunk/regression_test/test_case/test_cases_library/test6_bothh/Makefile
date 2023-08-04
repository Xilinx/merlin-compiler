VARIABLE = ""
test:
	rm -rf err.log
	touch err.log
	merlincc -c *.c -device=aocl::a10gx_es3 > log
	perl check.pl
