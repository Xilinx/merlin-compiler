#CASE=cnn_all.cpp
CASE=cnn_1d_array.cpp

#run:
#	/curr/xuechao/prog/Merlin_Compiler/trunk/source-opt/tools/mars_opt/src/mars_opt ${CASE} -e c -p nn_dse -a impl_num=1 -a search_step=1

run:
	mars_opt ${CASE} -e c -p systolic_frontend -a impl_num=1 -a search_step=1

clean:
	rm -f rose* *.o systolic_array_kernel* *.log *.ti *.json .finish_read_files
