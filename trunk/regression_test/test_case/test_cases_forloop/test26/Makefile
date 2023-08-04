test:
	rm -rf err.log
	touch err.log
#rm -rf ii_last.log
#	if [-e "ii.log"]; then mv ii.log ii_last.log; fi
#	touch ii.log
	aoc --fp-relaxed -g -c kernel_top.cl --report
	aoc --fp-relaxed -g -c kernel_flann.cl --report
	cp ../forloop_test.pl ./
	perl forloop_test.pl





