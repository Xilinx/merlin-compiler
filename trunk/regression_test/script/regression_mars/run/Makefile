
all_front:
	python regression.py case.list mars_front.py config.xml | tee regression_front.log

all_front_cloud:
	python regression.py case.list mars_front_cloud.py config.xml | tee regression_front.log

one_front:
	python regression.py one.list mars_front.py config.xml

one_front_cloud:
	python regression.py one.list mars_front_cloud.py config.xml



all_sim:
	python regression.py case.list mars_sim.py config.xml -k | tee regression_sim.log

all_sim_cloud:
	python regression.py case.list mars_sim_cloud.py config.xml -k | tee regression_sim.log

one_sim:
	python regression.py one.list mars_sim.py config.xml -k

one_sim_cloud:
	python regression.py one.list mars_sim_cloud.py config.xml -k



one_hls:
	python regression.py one.list mars_hls.py config.xml

one_hls_cloud:
	python regression.py one.list mars_hls_cloud.py config.xml

all_hls:
	python regression.py case.list mars_hls.py config.xml | tee regression_hls.log

all_hls_cloud:
	python regression.py case.list mars_hls_cloud.py config.xml | tee regression_hls.log

all_estimate_bit:
	python regression.py case.list mars_estimate_bit.py config.xml | tee regression_estimate_bit.log

all_estimate_bit_cloud:
	python regression.py case.list mars_estimate_bit_cloud.py config.xml | tee regression_estimate_bit.log

one_bit:
	python regression.py one.list mars_bit.py config.xml

one_bit_cloud:
	python regression.py one.list mars_bit_cloud.py config.xml

all_bit:
	python regression.py case.list mars_bit.py config.xml | tee regression_bit.log

all_bit_cloud:
	python regression.py case.list mars_bit_cloud.py config.xml | tee regression_bit.log


one_test:
	python regression.py one.list mars_test.py config.xml -k

all_test:
	python regression.py case.list mars_test.py config.xml -k | tee regression_test.log

all_no_bit_test:
	python regression.py case.list mars_no_bit_test.py config.xml -k | tee regression_host.log

one_test_cloud:
	python regression.py one.list mars_test_cloud.py config.xml -k

all_test_cloud:
	python regression.py case.list mars_test_cloud.py config.xml -k | tee regression_host.log

all_no_bit_test_cloud:
	python regression.py case.list mars_no_bit_test_cloud.py config.xml -k | tee regression_host.log

all_host:
	python regression.py case.list mars_host.py config.xml -k | tee regression_host.log

all_host_cloud:
	python regression.py case.list mars_host_cloud.py config.xml -k | tee regression_host.log

one_host:
	python regression.py one.list mars_host.py config.xml -k

one_host_cloud:
	python regression.py one.list mars_host_cloud.py config.xml -k


one_gen:
	python regression.py one.list mars_gen.py config.xml | tee regression_gen.log

all_gen:
	python regression.py case.list mars_gen.py config.xml | tee regression_gen.log

all_ref:
	rm -rf test_ref
	mkdir  test_ref
	python regression.py case.list mars_ref.py config.xml -k | tee regression_ref.log

all_diff:
	python regression.py case.list mars_diff.py config.xml -k | tee regression_ref.log




