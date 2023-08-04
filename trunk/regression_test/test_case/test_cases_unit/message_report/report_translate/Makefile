test:
	-rm -rf err.log
	-rm -rf run
	-touch err.log
	pytest || (echo 'message report translate module fail' >> err.log ; exit 1;)
