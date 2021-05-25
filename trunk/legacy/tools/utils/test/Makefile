
SHELL = "/bin/csh"
#SHELL = "/bin/sh"


"ESLOPT_HOME=$src_top \n";


go:
	@echo "ESLOPT_HOME=${ESLOPT_HOME}"
	perl ${ESLOPT_HOME}/scripts/src_config.pl test_p1.c -x test.cfg.xml -o test_out.c debug
	cat test_out.c

go1:
	@echo "ESLOPT_HOME=${ESLOPT_HOME}"
	perl ${ESLOPT_HOME}/scripts/src_config.pl test_p1.c -x test1.cfg.xml -o test_out.c debug
	cat test_out.c
