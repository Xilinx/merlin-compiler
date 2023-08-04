
unit_test:
	-rm -rf err.log
	touch err.log
	python start_make.py
	#find . -depth -name run -type d -exec rm -fr {} \;

test:
	-rm -rf err.log
	touch err.log
	python start_make.py
	#find . -depth -name run -type d -exec rm -fr {} \;

fast_update:
	python update_on_local.py

update:
	python update_by_condor.py update file_list

copy_back:
	python update_by_condor.py copy_back file_list

clean:
	find . -depth -name run -type d -exec rm -fr {} \;
	find . -name '*.log' -exec rm {} \;
	find . -name '*.json' -exec rm {} \;
	find . -name '*.out' -exec rm {} \;
