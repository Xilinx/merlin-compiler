CASES=$(notdir $(basename $(wildcard src/*.cpp)))

define run_model
	@vendor=$(1); \
	for case in ${CASES}; do \
		mars_opt src/$$case.cpp -e c -p ds_resource_eval -a impl_tool=$$vendor; \
		mv lut_memctrl.json lut_$${vendor}_$${case}.json; \
		mv usage.txt bram_$${vendor}_$${case}.txt; \
	done
endef

define check_result
	@vendor=$(1); \
	for case in ${CASES}; do \
		diff bram_$${vendor}_$${case}.txt refs/$${vendor}/bram_$${vendor}_$${case}.txt &>> err.log; \
		python ../common/check_json.py lut_$${vendor}_$${case}.json refs/$${vendor}/lut_$${vendor}_$${case}.json &>> err.log; \
	done
endef

run: 
	$(call run_model,sdaccel)
	$(call run_model,aocl)

update: run
	@mv *_sdaccel_* refs/sdaccel
	@mv *_aocl_* refs/aocl

diff: 
	$(call run_model,sdaccel)
	$(call check_result,sdaccel)
	$(call run_model,aocl)
	$(call check_result,aocl)


clean:
	find . -name "rose*"  -prune -exec rm -rf {} \;
	find . -name "flex*"  -prune -exec rm -rf {} \;
	find . -name "*.log"  -prune -exec rm -rf {} \;
	find . -name "*.rpt"  -prune -exec rm -rf {} \;
	find . -name "history*.json" -prune -exec rm -rf {} \;
	find . -name "temp"   -prune -exec rm -rf {} \; 
	find . -name ".*idx*" -prune -exec rm -rf {} \; 
	rm -rf *.txt; 
	rm -rf *.json;
	find . -name ".finish_read_files" -prune -exec rm -rf {} \; 
