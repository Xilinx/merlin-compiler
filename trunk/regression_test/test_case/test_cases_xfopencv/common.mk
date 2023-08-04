TARGET=mcc_acc

test:
	mkdir -p $(CASE)
	mkdir -p $(CASE)/../src
	cp -r $(MERLIN_COMPILER_HOME)/regression_test/merlin_benchmark/common .
	cp -r $(MERLIN_COMPILER_HOME)/regression_test/merlin_benchmark/xfopencv/include xfopencv
	cp -r $(MERLIN_COMPILER_HOME)/regression_test/merlin_benchmark/$(CASE)/../src $(CASE)/..
	cp -r $(MERLIN_COMPILER_HOME)/regression_test/merlin_benchmark/$(CASE)/Makefile $(CASE)/Makefile
	rm -rf err.log
	touch err.log
	make -C $(CASE) $(TARGET) || echo "fails" >> err.log
