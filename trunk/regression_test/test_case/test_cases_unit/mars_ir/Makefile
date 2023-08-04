
unit_test: init
	make compare
run: run_ir run_ir_v2 run_rpt

init: 
	-rm -rf err.log
	-touch err.log

run_ir:
	-mars_opt test_ir/test.cpp -e c -p test_mars_ir | tee test.cpp.log
	-mars_opt test_ir/test_pragma_out.cpp -e c -p test_mars_ir | tee test_pragma_out.cpp.log
	-mars_opt test_ir/test_small_memcpy.cpp -e c -p test_mars_ir | tee test_small_memcpy.cpp.log

run_ir_v2:
	-mars_opt test_ir/test.cpp -e c -p test_mars_ir_v2 | tee test_v2.cpp.log
	-mars_opt test_ir/test_call.cpp -e c -p test_mars_ir_v2 | tee test_call_v2.cpp.log
	-mars_opt test_ir/test_if.cpp -e c -p test_mars_ir_v2 | tee test_if_v2.cpp.log
run_rpt:
	-mars_opt test_ir/test_err.cpp -e c -p test_mars_ir_v2
	-mars_opt test_ir/test_err_1.cpp -e c -p test_mars_ir_v2
	-mars_opt test_ir/test_err_2.cpp -e c -p test_mars_ir_v2
	-mars_opt test_ir/test_pragma_err.cpp -e c -p test_mars_ir
	-mars_opt test_ir/test_err.cpp -e c -p test_mars_ir
	-mars_opt test_ir/test_err_1.cpp -e c -p test_mars_ir
	-mars_opt test_ir/test_err_2.cpp -e c -p test_mars_ir
	-mars_opt test_ir/test_err_3.cpp -e c -p test_mars_ir

compare: run
	-diff --brief test.cpp.log test_rule/test.cpp.log.ref &>> err.log  
	-diff --brief test_pragma_out.cpp.log test_rule/test_pragma_out.cpp.log.ref &>> err.log  
	-diff --brief test_small_memcpy.cpp.log test_rule/test_small_memcpy.cpp.log.ref &>> err.log  
	-diff --brief test_v2.cpp.log test_rule/test_v2.cpp.log.ref &>> err.log
	-diff --brief test_call_v2.cpp.log test_rule/test_call_v2.cpp.log.ref &>> err.log
	-diff --brief test_if_v2.cpp.log test_rule/test_if_v2.cpp.log.ref &>> err.log
	-diff --brief critical_message.rpt test_rule/critical_message.rpt.ref &>> err.log

update: run	
	-cp test.cpp.log test_rule/test.cpp.log.ref       
	-cp test_pragma_out.cpp.log test_rule/test_pragma_out.cpp.log.ref
	-cp test_small_memcpy.cpp.log test_rule/test_small_memcpy.cpp.log.ref
	-cp test_v2.cpp.log test_rule/test_v2.cpp.log.ref  
	-cp test_call_v2.cpp.log test_rule/test_call_v2.cpp.log.ref
	-cp test_if_v2.cpp.log test_rule/test_if_v2.cpp.log.ref  
	-cp critical_message.rpt test_rule/critical_message.rpt.ref


