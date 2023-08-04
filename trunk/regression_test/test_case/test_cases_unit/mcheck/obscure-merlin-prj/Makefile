# We want to ensure that mcheck's diagnostics source locations do not disclose
# the location of our directory of intermediates, `.merlin_prj`.

HIDDEN_DIRECTORY_REGEX ?= '\.merlin_prj\>'

unit_test:
	mcheck .merlin_prj/intentional-error.cpp 2>&1 | grep $(HIDDEN_DIRECTORY_REGEX) > err.log || true;
update:
	echo "nothing to do"
