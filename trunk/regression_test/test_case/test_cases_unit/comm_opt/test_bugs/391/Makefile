test:
	rm -rf err.log
	touch err.log
	-mars_opt testerr.c -e c -p comm_token || echo fails >> err.log
