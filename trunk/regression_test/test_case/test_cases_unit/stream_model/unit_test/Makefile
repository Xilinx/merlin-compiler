all:
	-mars_opt test_cg_par.c -e c -p test_stream_ir -a test_codegen | tee test_cg_par.out.log
	-mars_opt test_codegen.c -e c -p test_stream_ir -a test_codegen | tee test_codegen.out.log
	-mars_opt test_codegen_func.c -e c -p test_stream_ir -a test_codegen | tee test_codegen_func.out.log
	-mars_opt test_port_gen.c -e c -p test_stream_ir -a test_port_gen | tee test_port_gen.out.log
	-mars_opt test_spawn.c -e c -p test_stream_ir -a test_spawn | tee test_spawn.out.log
diff:
	-diff -du rose_test_cg_par.c test_cg_par.out.ref &>> ../err.log
	-diff -du rose_test_codegen.c test_codegen.out.ref &>> ../err.log
	-diff -du rose_test_codegen_func.c test_codegen_func.out.ref &>> ../err.log
	-diff -du rose_test_port_gen.c test_port_gen.out.ref &>> ../err.log
	-diff -du rose_test_spawn.c test_spawn.out.ref &>> ../err.log
update:
	cp rose_test_cg_par.c test_cg_par.out.ref 
	cp rose_test_codegen.c test_codegen.out.ref 
	cp rose_test_codegen_func.c test_codegen_func.out.ref 
	cp rose_test_port_gen.c test_port_gen.out.ref 
	cp rose_test_spawn.c test_spawn.out.ref 
