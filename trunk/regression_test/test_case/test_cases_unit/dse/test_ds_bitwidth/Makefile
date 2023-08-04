run:
	@mars_opt main.cpp vec_add_kernel.cpp  -e c -p ds_bitwidth_opt; \
		mv rose_vec_add_kernel.cpp rose_full_kernel.cpp

update: run
	@cp -f rose_full_kernel.cpp ref_full_kernel.cpp

diff: run
	@diff --brief rose_full_kernel.cpp ref_full_kernel.cpp &>> err.log

clean:
	find . -name "rose*"  -prune -exec rm -rf {} \;
	find . -name "flex*"  -prune -exec rm -rf {} \;
	find . -name "*.log"  -prune -exec rm -rf {} \;
	find . -name "*.rpt"  -prune -exec rm -rf {} \;
	find . -name "*.json" -prune -exec rm -rf {} \;
	find . -name "temp"   -prune -exec rm -rf {} \;
	find . -name ".*idx*" -prune -exec rm -rf {} \; 
