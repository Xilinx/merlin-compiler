cases = data_transfer_gen_s.c \
        data_transfer_gen_full.c \
        data_transfer_gen_return.c \
        data_transfer_gen_enum.c \
        data_transfer_gen_ap_int.cpp \
		data_transfer_gen_struct_depth.c \
		data_transfer_gen_int128.cpp \
		data_transfer_gen_wchar.cpp  \
		data_transfer_gen_max_depth.c
unit_test: $(patsubst %,diff-%,$(cases))
	cat $(patsubst %,%.err.log,$(cases)) > err.log

all_test:   build unit_test
gen_ref: $(patsubst %,update-%,$(cases))
diff-%: out-%
	$(eval $@_src_file := $(patsubst diff-%,%,$@))
	-diff --brief  rose_${$@_src_file} reference/rose_${$@_src_file} &>> ${$@_src_file}.err.log 2>&1
out-%:
	$(eval $@_src_file := $(patsubst out-%,%,$@))
	rm -rf  ${$@_src_file}.err.log; touch  ${$@_src_file}.err.log;
	-mars_opt ${$@_src_file} -e c -p interface_transform -a impl_tool=sdaccel \
	2>${$@_src_file}.tmp.log || (cat ${$@_src_file}.tmp.log >> ${$@_src_file}.err.log)

update-%: out-%
	$(eval $@_src_file := $(patsubst update-%,%,$@))
	-cp rose_${$@_src_file} reference/rose_${$@_src_file} 


clean:
	rm -rf rose* *.o tmp.log err.log flex*.log history_*.json


build:
	source ~/fcs_setting64.sh; cd ~/WORK2; make all; cd -

gdb:
	-/usr/bin/gdb --args mars_opt_org data_transfer_gen_s.c -e c -p interface_transform
