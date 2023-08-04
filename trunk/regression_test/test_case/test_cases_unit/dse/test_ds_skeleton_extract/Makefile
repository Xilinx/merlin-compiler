run:
	@mars_opt aes_kernel.cpp -e c -p ds_skeleton_extract;

update: run
	@cp -f skeleton.json ref_skeleton.json

diff: run
	@python ../common/check_json.py skeleton.json ref_skeleton.json &>> err.log

clean:
	find . -name "rose*"  -prune -exec rm -rf {} \;
	find . -name "flex*"  -prune -exec rm -rf {} \;
	find . -name "*.log"  -prune -exec rm -rf {} \;
	find . -name "*.rpt"  -prune -exec rm -rf {} \;
	find . -name "temp"   -prune -exec rm -rf {} \; 
	find . -name ".*idx*" -prune -exec rm -rf {} \; 
	find . -name "skeleton.json" -prune -exec rm -rf {} \; 
	find . -name "history*.json" -prune -exec rm -rf {} \; 
	find . -name ".finish_read_files" -prune -exec rm -rf {} \; 

