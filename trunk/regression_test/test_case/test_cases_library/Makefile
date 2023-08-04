
unit_test:
	rm -rf err.log
	touch err.log
	cd test1_integerbuiltin;	make test;	cat "err.log" >> ../err.log
	cd test1_math;			 	make test;	cat "err.log" >> ../err.log
	cd test2_c99; 				make test;	cat "err.log" >> ../err.log
	cd test3_orgmath; 			make test;	cat "err.log" >> ../err.log
	cd test4_orgmath_pop; 		make test;	cat "err.log" >> ../err.log
	cd test5_misfuncdef; 		make test;	cat "err.log" >> ../err.log
	cd test6_bothh; 			make test;	cat "err.log" >> ../err.log
	cd test7_misdecl; 			make test;	cat "err.log" >> ../err.log
	cd test7_misdecl2; 			make test;	cat "err.log" >> ../err.log
	cd test7_misdecl3; 			make test;	cat "err.log" >> ../err.log
	cd test8_c11; 				make test;	cat "err.log" >> ../err.log
