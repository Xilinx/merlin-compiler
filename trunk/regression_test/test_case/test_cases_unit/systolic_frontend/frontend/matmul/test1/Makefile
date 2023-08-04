CASE=test1.cpp

run:
	mars_opt ${CASE} -e c -p systolic_frontend -a impl_num=1 -a search_step=1 |& tee run.log

clean:
	rm -f rose* *.o systolic_array_kernel* *.log *.ti *.json .finish_read_files golden.cpp
