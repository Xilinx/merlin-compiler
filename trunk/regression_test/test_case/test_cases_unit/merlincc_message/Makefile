
#$(eval is_xilinx_18_2=$(shell sdx --version | grep "v2018.2"))
#$(eval is_xilinx_16_4=$(shell sdx --version | grep "v2016.4"))
#$(eval is_xilinx_18_2=$(shell sdx --version | grep "v2018.2"))
#
#ifneq ("$(is_xilinx_18_2)", "")
#	DEVICE=xilinx_vcu1525_dynamic_5_0
#else ifneq ("$(is_xilinx_16_4)", "")
#	DEVICE=xilinx:minotaur-vu9p-f1:4ddr-xpr:3.3
#else ifneq ("$(is_xilinx_18_2)", "")
#	DEVICE=xilinx_vcu1525_dynamic_5_1
#endif
#DEVICE=xilinx_vcu1525_dynamic_5_1
DEVICE=auto

correct_xilinx= test1
incorrect_xilinx= test2
unit_test: run  
run: clean
	-rm -rf err.log
	-touch err.log
	-for name in `echo $(correct_xilinx)`; \
	do \
		cd $$name; touch err.log; cd -; \
		cd $$name; merlincc src/vec_add_kernel.cpp -march=sw_emu -d11 --debug_report; cd -; \
		if [ ! -f "$$name/.Mer/merlin_sdaccel.log" ]; then \
			echo "$$name fail to generate merlin_sdaccel.log aa" >> $$name/err.log; \
		fi; \
		if [ ! -f "$$name/.Mer/vec_add_kernel/vivado_hls.log" ]; then \
			echo "$$name fail to generate vivado_hls.log aa" >> $$name/err.log; \
		fi; \
		cd $$name; merlincc src/vec_add_kernel.cpp --report=estimate -d11 --debug_report; cd -; \
		if [ ! -f "$$name/.Mer/merlin_sdaccel.log" ]; then \
			echo "$$name fail to generate merlin_sdaccel.log bb" >> $$name/err.log; \
		fi; \
		if [ ! -f "$$name/.Mer/vec_add_kernel/vivado_hls.log" ]; then \
			echo "$$name fail to generate vivado_hls.log bb" >> $$name/err.log; \
		fi; \
		cat $$name/err.log >> err.log; \
	done
	-for name in `echo $(incorrect_xilinx)`; \
	do \
		cd $$name; touch err.log; cd -; \
		cd $$name; merlincc -c src/vec_add_kernel.cpp --report=estimate -d11 --debug_report; cd -; \
		if [ ! -f "$$name/.Mer/merlin_sdaccel.log" ]; then \
			echo "$$name fail to generate merlin_sdaccel.log aaa" >> $$name/err.log; \
		fi; \
		if [ ! -f "$$name/.Mer/vec_add_kernel/vivado_hls.log" ]; then \
			echo "$$name fail to generate vivado_hls.log bbb" >> $$name/err.log; \
		fi; \
		cd $$name; merlincc kernel_top.mco -march=sw_emu -d11 --debug_report; cd -; \
		if [ ! -f "$$name/.Mer/merlin_sdaccel.log" ]; then \
			echo "$$name fail to generate merlin_sdaccel.log ccc" >> $$name/err.log; \
		fi; \
		if [ ! -f "$$name/.Mer/vec_add_kernel/vivado_hls.log" ]; then \
			echo "$$name fail to generate vivado_hls.log ddd" >> $$name/err.log; \
		fi; \
		cat $$name/err.log >> err.log; \
	done

clean:
	find . -name "*.json" -exec rm -rf {} \;
	find . -name "pass.o*" -exec rm -rf {} \;
	find . -name "fail.o" -exec rm -rf {} \;
	find . -name "*.log" -exec rm -rf {} \;
	find . -name "log*" -exec rm -rf {} \;
	find . -name "*.rpt" -exec rm -rf {} \;
	find . -name "test_*" -exec rm -rf {} \;
	find . -name "*.h" -exec rm -rf {} \;
	find . -name "pass.o" -exec rm -rf {} \;
	find . -name "lib*" -exec rm -rf {} \;
	find . -name "flex*" -exec rm -rf {} \;
	find . -name "sdaccel*.csv" -exec rm -rf {} \;
	find . -name "profile.mon" -exec rm -rf {} \;
	-for name in $(correct_xilinx); \
	do \
		cd $$name; rm -rf .merlin_prj; rm -rf .Mer; cd -; \
	done
	-for name in $(incorrect_xilinx); \
	do \
		cd $$name; rm -rf .merlin_prj; rm -rf .Mer; cd -; \
	done
	rm -rf err.log
