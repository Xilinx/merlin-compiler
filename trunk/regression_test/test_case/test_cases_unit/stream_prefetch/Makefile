
case_groups=kernel_wrapper stream_prefetch final_code_gen stream_prefetch_xilinx 
unit_test: $(patsubst %,run-%,${case_groups}) 
	cat $(patsubst %,%/err.log,${case_groups}) > err.log 
run-%:
	$(eval $@_src_file := $(patsubst run-%,%,$@))
	-make -C ${$@_src_file} -j
update: $(patsubst %,update-%,$(case_groups))
update-%:
	$(eval $@_src_file := $(patsubst update-%,%,$@))
	-make update -C ${$@_src_file} -j

clean: $(patsubst %,clean-%,${case_groups})
clean-%:
	$(eval $@_src_file := $(patsubst clean-%,%,$@))
	-make clean -C ${$@_src_file} -j
	-rm -rf err.log
	-touch err.log
