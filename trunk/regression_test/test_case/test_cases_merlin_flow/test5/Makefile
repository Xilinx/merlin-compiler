test:
	rm -rf err.log
	touch err.log
	merlin_flow run
	if [ -d "run/implement/export_kestrel" -a -d "run/pkg_kestrel" ]; then echo "pass"; else echo "failed to run opt" >> err.log; fi
	merlin_flow run host
	if [ -f "run/pkg_kestrel/host/host_top" -a -f "run/pkg_kestrel/loader/loader.so" -a -f "run/pkg_kestrel/loader/nam_conf" ]; then echo "pass"; else echo "failed to run host" >> err.log; fi
