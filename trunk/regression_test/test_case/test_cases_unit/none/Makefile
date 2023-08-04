TESTS ?=\
	unused_label ap_int_api reference_interface default_macro \
	conflict_header hls_math_with_ap_int  ap_int_in_struct \
    empty_size_and_enum_value_size ap_axi_sdata_h ap_int_vec_add

unit_test: run

run: $(patsubst %,run-%,$(TESTS)) 
	cat $(patsubst %,%.err.log,$(TESTS)) >err.log

run-empty_size_and_enum_value_size: empty_size_and_enum_value_size.c
	$(eval $@_log_file := $(patsubst run-%,%.err.log,$@))
	rm -rf $($@_log_file) 
	touch $($@_log_file)
	-mkdir -p $(patsubst %.c,%.run,$^); cd $(patsubst %.c,%.run,$^) && \
	merlincc -I ${XILINX_VIVADO}/include -p=vitis::auto \
									 -o $(patsubst %.c,%_kernel, $^) -c ../$^ 2>&1 > $^.log || \
	                 (grep -o 'SYNCHK-302' $^.log 2>tmp.log || \
									 echo $^ "check out-of-bound failed" >> ../$($@_log_file));
run-conflict_header: conflict_header.cpp
	$(eval $@_log_file := $(patsubst run-%,%.err.log,$@))
	rm -rf $($@_log_file) 
	touch $($@_log_file)
	-mkdir -p $(patsubst %.cpp,%.run,$^); cd $(patsubst %.cpp,%.run,$^) && \
	merlincc --attribute pure_kernel=on -I ${XILINX_VIVADO}/include -p=vitis::auto \
									 -o $(patsubst %.cpp,%_kernel, $^) -c ../$^ 2>&1 || \
									 echo $^ failed >> ../$($@_log_file);
run-ap_axi_sdata_h: ap_axi_sdata_h.cpp
	$(eval $@_log_file := $(patsubst run-%,%.err.log,$@))
	rm -rf $($@_log_file) 
	touch $($@_log_file)
	-mkdir -p $(patsubst %.cpp,%.run,$^); cd $(patsubst %.cpp,%.run,$^) && \
	merlincc --attribute pure_kernel=on -I ${XILINX_VIVADO}/include -p=vitis::auto \
									 -o $(patsubst %.cpp,%_kernel, $^) -c ../$^ 2>&1 || \
									 echo $^ failed >> ../$($@_log_file);

run-unused_label: unused_label.cpp
	$(eval $@_log_file := $(patsubst run-%,%.err.log,$@))
	rm -rf $($@_log_file) 
	touch $($@_log_file)
	-mkdir -p $(patsubst %.cpp,%.run,$^); cd $(patsubst %.cpp,%.run,$^) && \
	merlincc --attribute pure_kernel=on -p=vitis::auto \
									 -o $(patsubst %.cpp,%_kernel, $^) -c ../$^ 2>&1  > $^.log || \
									 echo $^ failed >> ../$($@_log_file) && \
	grep -o 'warning:.\+$$' $^.log > ../$(patsubst %.cpp,%.out,$^) || true;
	diff -Nu $(patsubst %.cpp,%.expected,$^) $(patsubst %.cpp,%.out,$^) &>> ../$($@_log_file) || true
run-%: %.cpp
	$(eval $@_log_file := $(patsubst run-%,%.err.log,$@))
	rm -rf $($@_log_file) 
	touch $($@_log_file)
	-mkdir -p $(patsubst %.cpp,%.run,$^); cd $(patsubst %.cpp,%.run,$^) && \
	merlincc --attribute pure_kernel=on -p=vitis::auto \
									 -o $(patsubst %.cpp,%_kernel, $^) -c ../$^ 2>&1 || \
									 echo $^ failed >> ../$($@_log_file);
update: $(patsubst %,update-%,$(TESTS))
update-unused_label: unused_label.cpp
	mkdir -p $(patsubst %.cpp,%.run,$^); cd $(patsubst %.cpp,%.run,$^) && \
	merlincc --attribute pure_kernel=on -p=vitis::auto \
									 -o $(patsubst %.cpp,%_kernel, $^) -c ../$^ 2>&1 | \
	grep -o 'warning:.\+$$' > ../$(patsubst %.cpp,%.expected,$^) || true;

update-%: %.cpp

clean:
	rm -rf *.diff *.out err.log update-* diff-* *.log *.run
