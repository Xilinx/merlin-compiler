TESTS ?=\
      test test2 test3 test4 test5

unit_test: $(patsubst %,diff-%,$(TESTS)) 
	cat \
		$(patsubst %,%.diff,$(TESTS)) \
		> err.log

update: $(patsubst %,update-%,$(TESTS))

update-%: %.cpp
	source ${MERLIN_COMPILER_HOME}/set_env/env.sh; mrefactor $^ 
	cp -r $^  $(patsubst %.cpp,%.expected,$^) 
	cp -r $(patsubst %.cpp,%.orig,$^) $^ 
	source ${MERLIN_COMPILER_HOME}/set_env/env.sh; mrefactor -revert $(patsubst %.cpp,%_r.cpp,$^) 
	cp -r $(patsubst %.cpp,%_r.cpp,$^)  $(patsubst %.cpp,%_r.expected,$^) 
	cp -r $(patsubst %.cpp,%_r.orig,$^) $(patsubst %.cpp,%_r.cpp,$^)

diff-%: %.cpp
	source ${MERLIN_COMPILER_HOME}/set_env/env.sh; mrefactor $^ 
	diff -Nu $(patsubst %.cpp,%.expected,$^) $^ > $(patsubst %.cpp,%.diff,$^)
	cp -r $(patsubst %.cpp,%.orig,$^) $^ 
	source ${MERLIN_COMPILER_HOME}/set_env/env.sh; mrefactor -revert $(patsubst %.cpp,%_r.cpp,$^) 
	diff -Nu $(patsubst %.cpp,%_r.expected,$^) $(patsubst %.cpp,%_r.cpp,$^) > $(patsubst %.cpp,%_r.diff,$^) ||true
	cp -r $(patsubst %.cpp,%_r.orig,$^) $(patsubst %.cpp,%_r.cpp,$^)


clean:
	rm -f *.diff *.out err.log
