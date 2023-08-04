
build:
	make -C ..

all_test:   build unit_test

unit_test: diff

all_run:
	-rm -rf err.log critical_message.rpt
	-rm -rf rose*
	-mars_opt test_case/test_aocl_1.cpp -e c -p channel_partition -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_case/test_aocl_2.cpp -e c -p channel_partition -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_case/test_aocl_3.cpp -e c -p channel_partition -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_case/test_aocl_4.cpp -e c -p channel_partition -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	#-mars_opt test_case/test_aocl_5.cpp -e c -p channel_partition -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_case/test_aocl_6.cpp -e c -p channel_partition -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_case/test_user_channel.cpp -e c -p channel_partition -a impl_tool=aocl 2>tmp.log || (cat tmp.log >> err.log)
	-mars_opt test_case/test_illegal.cpp -e c -p channel_partition -a impl_tool=aocl &> illegal.log
	grep -i "Assertion" illegal.log || echo "test_illegal fails" >> err.log

diff: all_run
	-diff --brief rose_test_aocl_1.cpp test_rule/rose_test_aocl_1.cpp &>> err.log
	-diff --brief rose_test_aocl_2.cpp test_rule/rose_test_aocl_2.cpp &>> err.log
	-diff --brief critical_message.rpt test_rule/critical_message.ref &>> err.log
	-diff --brief rose_test_aocl_4.cpp test_rule/rose_test_aocl_4.cpp &>> err.log
	#-diff --brief rose_test_aocl_5.cpp test_rule/rose_test_aocl_5.cpp &>> err.log
	-diff --brief rose_test_aocl_6.cpp test_rule/rose_test_aocl_6.cpp &>> err.log
	-diff --brief rose_test_user_channel.cpp test_rule/rose_test_user_channel.cpp &>> err.log

update: all_run
	-cp rose_test_aocl_1.cpp test_rule/rose_test_aocl_1.cpp
	-cp rose_test_aocl_2.cpp test_rule/rose_test_aocl_2.cpp
	-cp critical_message.rpt test_rule/critical_message.ref
	-cp rose_test_aocl_4.cpp test_rule/rose_test_aocl_4.cpp
#-cp rose_test_aocl_5.cpp test_rule/rose_test_aocl_5.cpp
	-cp rose_test_aocl_6.cpp test_rule/rose_test_aocl_6.cpp
	-cp rose_test_user_channel.cpp test_rule/rose_test_user_channel.cpp
