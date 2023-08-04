test:
	cp -r intel_dse intel_dse_bk
	python test.py
	rm -rf cd-*
	rm -rf cp-*
	rm -rf diff-*
	rm -rf make-*
clean:
	rm -rf intel_dse
	mv intel_dse_bk intel_dse
	rm command.log
	rm err.log
	#rm -rf  merlincc_report  *.mco *.log *err *out __*
