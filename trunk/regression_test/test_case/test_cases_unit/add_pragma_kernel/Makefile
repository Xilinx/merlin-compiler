unit_test: clean
	mars_opt -e c -p add_pragma_kernel test.cpp
	diff rose_test.cpp test.cpp.ref >> err.log
update:	
	cp rose_test.cpp test.cpp.ref

clean:
	-rm rose*
	-rm *.log
	-rm *.json
