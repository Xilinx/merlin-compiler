unit_test: diff
init:
	rm -rf err.log
	touch err.log
run: init
	-mars_opt denoise2D.c -e c -p test_mars_ir 2>tmp.log | tee mars_ir.log || echo tmp.log >> err.log
	-mars_opt denoise2D.c -e c -p test_mars_ir_v2 2>tmp.log | tee mars_ir_v2.log || echo tmp.log >> err.log
diff: run
	-diff --brief mars_ir.log mars_ir.log.ref &>>err.log
	-diff --brief mars_ir_v2.log mars_ir_v2.log.ref &>>err.log
update: run
	-cp mars_ir.log mars_ir.log.ref 
	-cp mars_ir_v2.log mars_ir_v2.log.ref 
