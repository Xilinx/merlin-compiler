test: diff
run:
	-rm -rf err.log
	-rm -rf critical_message.rpt
	-rm -rf rose*
	-touch err.log
	-mars_opt test_invalid_bitwidth.c  -e c -p bitwidth_opt -a impl_tool=sdaccel 2>tmp.log || (cat tmp.log >> err.log)
diff: run
	-diff --brief critical_message.rpt ../test_rule/critical_message.ref &>> err.log
update:
	-update critical_message.rpt ../test_rule/critical_message.ref

