NPROCS = $(shell grep -c 'processor' /proc/cpuinfo)
MAKEFLAGS += -j$(NPROCS)
type_list=char short int float double
bw_list=32 64 128 256 512
size_list=2000 2001
test:test1d test2d test3d test4d test5d
	rm -rf err.log
	cat err1.log >>err.log
	cat err2.log >>err.log
	cat err3.log >>err.log
	cat err4.log >>err.log
	cat err5.log >>err.log
test1d:
	rm -rf err1.log
	touch err1.log
	-for t in ${type_list}; \
	  do \
	   for s in ${size_list}; \
	     do \
		   for b in ${bw_list}; \
		 do \
		 mars_opt vec_add.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=$$s" -a bus_bitwidth=$$b; \
	     gcc -o 1d -O3 rose_vec_add.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./1d || echo "1d with $$t, $$s, $$b failed" >> err1.log; \
		 done \
		 done \
		 done
test2d:
	rm -rf err2.log
	touch err2.log
	-for t in ${type_list}; \
	  do \
		   for b in ${bw_list}; \
		 do \
		 mars_opt vec_add_2d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=2000 -DSIZE_1=250 -DSIZE_2=8" -a bus_bitwidth=$$b; \
	     gcc -o 2d -O3 rose_vec_add_2d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./2d || echo "2d with $$t, $$b failed" >> err2.log; \
		 mars_opt vec_add_2d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=1600 -DSIZE_1=50 -DSIZE_2=32" -a bus_bitwidth=$$b; \
	     gcc -o 2d -O3 rose_vec_add_2d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./2d || echo "2d with $$t, $$b failed" >> err2.log; \
		 done \
		 done 
test3d:
	rm -rf err3.log
	touch err3.log
	-for t in ${type_list}; \
	  do \
		   for b in ${bw_list}; \
		 do \
		 mars_opt vec_add_3d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=1600 -DSIZE_1=25 -DSIZE_2=8 -DSIZE_3=8" -a bus_bitwidth=$$b; \
	     gcc -o 3d -O3 rose_vec_add_3d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./3d || echo "3d with $$t, $$b failed" >> err3.log; \
		 mars_opt vec_add_3d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=1600 -DSIZE_1=25 -DSIZE_2=4 -DSIZE_3=16" -a bus_bitwidth=$$b; \
	     gcc -o 3d -O3 rose_vec_add_3d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./3d || echo "3d 2 with $$t, $$b failed" >> err3.log; \
		 mars_opt vec_add_3d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=1600 -DSIZE_1=25 -DSIZE_2=2 -DSIZE_3=32" -a bus_bitwidth=$$b; \
	     gcc -o 3d -O3 rose_vec_add_3d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./3d || echo "3d 3 with $$t, $$b failed" >> err3.log; \
		 done \
		 done 
test4d:
	rm -rf err4.log
	touch err4.log
	-for t in ${type_list}; \
	  do \
		   for b in ${bw_list}; \
		 do \
		 mars_opt vec_add_4d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=1600 -DSIZE_1=25 -DSIZE_2=4 -DSIZE_3=2 -DSIZE_4=8" -a bus_bitwidth=$$b; \
	     gcc -O3 -o 4d rose_vec_add_4d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./4d || echo "4d with $$t, $$b failed" >> err4.log; \
		 mars_opt vec_add_4d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=1600 -DSIZE_1=25 -DSIZE_2=2 -DSIZE_3=2 -DSIZE_4=16" -a bus_bitwidth=$$b; \
	     gcc -O3 -o 4d rose_vec_add_4d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./4d || echo "4d 2 with $$t, $$b failed" >> err4.log; \
		 mars_opt vec_add_4d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=1600 -DSIZE_1=25 -DSIZE_2=1 -DSIZE_3=2 -DSIZE_4=32" -a bus_bitwidth=$$b; \
	     gcc -O3 -o 4d rose_vec_add_4d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./4d || echo "4d 3 with $$t, $$b failed" >> err4.log; \
		 mars_opt vec_add_4d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=1600 -DSIZE_1=25 -DSIZE_2=1 -DSIZE_3=1 -DSIZE_4=64" -a bus_bitwidth=$$b; \
	     gcc -O3 -o 4d rose_vec_add_4d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./4d || echo "4d 4 with $$t, $$b failed" >> err4.log; \
		 done \
		 done 
test5d:
	rm -rf err5.log
	touch err5.log
	-for t in ${type_list}; \
	  do \
		   for b in ${bw_list}; \
		 do \
		 mars_opt vec_add_5d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=1280 -DSIZE_1=10 -DSIZE_2=2 -DSIZE_3=4 -DSIZE_4=2 -DSIZE_5=8" -a bus_bitwidth=$$b; \
	     gcc -o 5d -O3 rose_vec_add_5d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./5d || echo "5d with $$t, $$b failed" >> err5.log; \
		 mars_opt vec_add_5d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=2560 -DSIZE_1=10 -DSIZE_2=2 -DSIZE_3=4 -DSIZE_4=2 -DSIZE_5=16" -a bus_bitwidth=$$b; \
	     gcc -o 5d -O3 rose_vec_add_5d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./5d || echo "5d 2 with $$t, $$b failed" >> err5.log; \
		 mars_opt vec_add_5d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=2560 -DSIZE_1=40 -DSIZE_2=2 -DSIZE_3=4 -DSIZE_4=2 -DSIZE_5=4" -a bus_bitwidth=$$b; \
	     gcc -o 5d -O3 rose_vec_add_5d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./5d || echo "5d 3 with $$t, $$b failed" >> err5.log; \
		 mars_opt vec_add_5d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=2560 -DSIZE_1=20 -DSIZE_2=8 -DSIZE_3=4 -DSIZE_4=2 -DSIZE_5=2" -a bus_bitwidth=$$b; \
	     gcc -o 5d -O3 rose_vec_add_5d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./5d || echo "5d 4 with $$t, $$b failed" >> err5.log; \
		 mars_opt vec_add_5d.c -e c -p bitwidth_opt -a impl_tool=sdaccel \
	   -a cflags="-DDATA_T=$$t -DSIZE=2560 -DSIZE_1=5 -DSIZE_2=2 -DSIZE_3=2 -DSIZE_4=2 -DSIZE_5=64" -a bus_bitwidth=$$b; \
	     gcc -o 5d -O3 rose_vec_add_5d.c -std=c99 -I ${MERLIN_COMPILER_HOME}/mars-gen/drivers/code_transform/coalescing;\
		 ./5d || echo "5d with $$t, $$b failed" >> err5.log; \
		 done \
		 done 

