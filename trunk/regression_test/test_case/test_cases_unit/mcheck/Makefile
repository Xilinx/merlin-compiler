TESTS ?=\
	group-00 \
	member-pointers-00 \
	intrinsics-header \
	indirect-field-decl \
	pretty-function-00 \
    varidic_template_parameter \
    exception
FUNCTIONAL_TESTS ?= obscure-merlin-prj std-03 mer-1434 
#reject-ap-uints

unit_test: $(patsubst %,diff-%,$(TESTS)) $(patsubst %,functional-%,$(FUNCTIONAL_TESTS))
	cat \
		$(patsubst %,%.diff,$(TESTS)) \
		$(patsubst %,%/err.log,$(FUNCTIONAL_TESTS)) \
		> err.log

update: $(patsubst %,update-%,$(TESTS)) $(patsubst %,update-functional-%,$(FUNCTIONAL_TESTS))

update-%: %.cpp
	source ${MERLIN_COMPILER_HOME}/set_env/env.sh; mcheck $^ 2>&1 | grep -o 'error:.\+$$' > $(patsubst %.cpp,%.expected,$^)

diff-%: %.cpp
	source ${MERLIN_COMPILER_HOME}/set_env/env.sh; mcheck $^ 2>&1 | grep -o 'error:.\+$$' > $(patsubst %.cpp,%.out,$^) || true
	diff -Nu $(patsubst %.cpp,%.expected,$^) $(patsubst %.cpp,%.out,$^) > $(patsubst %.cpp,%.diff,$^) || true

functional-%:
	$(MAKE) -C $(patsubst functional-%,%,$@)
update-functional-%:
	$(MAKE) update -C $(patsubst update-functional-%,%,$@)
clean:
	rm -f *.diff *.out err.log
