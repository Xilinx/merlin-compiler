# single file tests
SINGLES ?=\
construct-01.cpp \
construct-02.cpp \
construct-03.cpp \
construct-04.cpp \
construct-05.cpp \
construct-06.cpp \
construct-07.cpp \
construct-08.cpp \
construct-09.cpp \
construct-10.cpp \
construct-default_param.cpp \
construct-11.cpp \
construct-12.cpp \
construct-13.cpp \
construct-14.cpp \
enum-01.cpp \
enum-02.cpp \
enum-03.cpp \
enum-04.cpp \
type-01.cpp \
function-pointer.cpp \
member-00.cpp \
member-01.cpp \
member-02.cpp \
member-03.cpp \
member-04.cpp \
member-05.cpp \
member-06.cpp \
member-07.cpp \
member-08.cpp \
member-09.cpp \
member-10.cpp \
member-11.cpp \
member-12.cpp \
member-function-01.cpp \
member-function-02.cpp \
member-function-03.cpp \
member-function-04.cpp \
member-function-05.cpp \
member-function-06.cpp \
member-function-07.cpp \
member-function-08.cpp \
overloaded-00.cpp \
overloaded-01.cpp \
overloaded-02.cpp \
overloaded-04.cpp \
overloaded-03.c \
templates-00.cpp \
templates-01.cpp \
templates-02.cpp \
templates-03.cpp \
templates-04.cpp \
templates-05.cpp \
templates-06.cpp \
template-class-00.cpp \
template-class-04.cpp \
template-class-05.cpp \
template-class-01.cpp \
template-class-02.cpp \
template_func_with_pramga.cpp \
template_member_func.cpp \
itanium-00.cpp \
itanium-01.cpp \
deletion-tagging-00.cpp \
ctors-00.cpp \
ctors-01.cpp \
ctors-02.cpp \
ctors-03.cpp \
ctors-04.cpp \
ctors-05.cpp \
ctors-06.cpp \
ctors-07.cpp \
ctors-09.cpp \
ctors-10.cpp \
copy-ctor-01.cpp \
default-params-00.cpp \
default-params-01.cpp \
default-params-02.cpp \
default-params-03.cpp \
default-params-04.cpp \
reserved-00.cpp \
iubk-unknown-node-00.cpp \
static-storage-00.cpp \
llvm-member-functions-test3.cpp \
typedef-00.cpp \
typedef-01.cpp \
ctor_connects.cpp \
temp-object-00.cpp  \
namespace-alias-01.cpp \
namespace-alias-02.cpp \
namespace-alias-03.cpp \
namespace-anonymous-01.cpp \
namespace-cls-decl-01.cpp \
namespace-cls-decl-02.cpp \
namespace-cls-decl-03.cpp \
namespace-cls-decl-04.cpp \
namespace-cls-decl-05.cpp \
namespace-cls-tmpl-01.cpp \
namespace-cls-tmpl-02.cpp \
namespace-cls-tmpl-03.cpp \
namespace-enum-decl-01.cpp \
namespace-enum-decl-02.cpp \
namespace-enum-decl-03.cpp \
namespace-ext-01.cpp \
namespace-ext-02.cpp \
namespace-ext-03.cpp \
namespace-fn-decl-01.cpp \
namespace-fn-decl-02.cpp \
namespace-fn-decl-03.cpp \
namespace-fn-decl-04.cpp \
namespace-fn-decl-05.cpp \
namespace-fn-decl-06.cpp \
namespace-fn-tmpl-01.cpp \
namespace-fn-tmpl-02.cpp \
namespace-fn-tmpl-03.cpp \
namespace-nested-01.cpp \
namespace-nested-02.cpp \
namespace-nested-03.cpp \
namespace-shadow-01.cpp \
namespace-shadow-02.cpp \
namespace-shadow-03.cpp \
namespace-typedef-decl-01.cpp \
namespace-typedef-decl-02.cpp \
namespace-typedef-decl-03.cpp \
namespace-var-decl-01.cpp \
namespace-var-decl-02.cpp \
namespace-var-decl-03.cpp \
namespace-simple-func.cpp \
namespace-pragma-01.cpp \
return_type.cpp           \
pragma-01.cpp             \
pragma-02.cpp             \
static-member-var-01.cpp  \
static-member-var-02.cpp  \
static-member-var-05.cpp  \
static-member-var-06.cpp  \
static-const-member-01.cpp \
static-member-fn-01.cpp   \
class-template-explicit-instantiation.cpp \
class-template-implicit-instantiation.cpp \
class-template-partial-specialization.cpp \
class-template-specialization.cpp \
function-template-explicit-instantiation.cpp \
function-template-implicit-instantiation.cpp \
function-template-specialization.cpp \
template-default-parameters.cpp \
template-dependent-names.cpp \
template-parameters.cpp \
friend_function.cpp \
empty_destruct.cpp \
decl-as-condition-01.cpp \
decl-as-condition-02.cpp \
decl-as-condition-03.cpp \
decl-as-condition-04.cpp \
decl-as-condition-05.cpp \
decl-as-condition-08.cpp \
decl-as-condition-09.cpp \
decl-as-condition-10.cpp \
simple-inheritence-01.cpp \
simple-inheritence-02.cpp \
simple-inheritence-03.cpp \
simple-inheritence-04.cpp \
simple-inheritence-05.cpp \
simple-inheritence-06.cpp \
simple-inheritence-07.cpp \
simple-inheritence-08.cpp \
simple-inheritence-09.cpp \
simple-inheritence-10.cpp \
simple-inheritence-11.cpp \
simple-inheritence-12.cpp \
typedef-case.cpp  \
xilinx_hls_stream.cpp \
friend_function_with_var.cpp \
function_pointer_as_template_argument.cpp
#decl-as-condition-06.cpp // ROSE crash
#decl-as-condition-07.cpp // ROSE bug (condition variable is renamed)


# error reporting tests.
REPORTS ?=\
unsupported-00.cpp \
unsupported-01.cpp \
unsupported-02.cpp \
unsupported-03.cpp \
type-00.cpp \
unsupported-type.cpp \
unsupported-interface.cpp \
static-member-var-03.cpp \
static-member-var-04.cpp \
unsupported-template-argument.cpp
# multiple file tests. each of these should live in their own subdirs containing
# their own Makefiles.
MULTI ?=\
multiple_files/cp_func \
multiple_files/cp_func_ol_func \
multiple_files/dup_func \
multiple_files/cls_in_header_file

PURE_KERNEL_LIST = kernel-interface.cpp
all:
	@echo "Usage: make [-j N] target_name"
	@echo "where target_name is one of:"
	@echo "    run: Run _all_ tests and record output correctness to err.log."
	@echo "    clean: Clean up intermediate artifacts of testing."
	@echo "    update: Generate _all_ .expected files."

# need to execute mars_opt in a temporary directory so that the resulting
# critical_message.rpt doesn't conflict with other parallel runs.
rpt-%:
	$(eval $@_src_file := $(shell readlink -f $(patsubst rpt-%,%,$@)))
	$(eval $@_crit_msg_dir := $(shell mktemp -d))
	pushd $($@_crit_msg_dir); \
		mars_opt $($@_src_file) -e c -p lower_cpp 2>/dev/null; \
		touch critical_message.rpt; \
		popd;
	mv $($@_crit_msg_dir)/critical_message.rpt $($@_src_file).rpt
	rm -rf $($@_crit_msg_dir)

diff-rpt-%: rpt-% %.rpt.expected
	$(eval $@_src_file := $(patsubst diff-rpt-%,%,$@))
	diff -u $($@_src_file).rpt $($@_src_file).rpt.expected > $($@_src_file).rpt.diff || true

out-%:
	$(eval $@_src_file := $(patsubst out-%,%,$@))
	(mars_opt $($@_src_file) -e c -p lower_cpp >$($@_src_file).log 2>&1) \
		&& (mv rose_$($@_src_file) $($@_src_file).out) \
		|| (mv $($@_src_file).log $($@_src_file).out);
	rm -f $($@_src_file).log rose_$($@_src_file)

diff-out-%: out-% %.expected
	$(eval $@_src_file := $(patsubst diff-out-%,%,$@))
	diff -u $($@_src_file).out $($@_src_file).expected > $($@_src_file).out.diff || true
out-p-%:
	$(eval $@_src_file := $(patsubst out-p-%,%,$@))
	(mars_opt $($@_src_file) -e c -p lower_cpp -a pure_kernel=on >$($@_src_file).log 2>&1) \
		&& (mv rose_$($@_src_file) $($@_src_file).out) \
		|| (mv $($@_src_file).log $($@_src_file).out);
	rm -f $($@_src_file).log rose_$($@_src_file)

diff-out-p-%: out-p-% %.expected
	$(eval $@_src_file := $(patsubst diff-out-p-%,%,$@))
	diff -u $($@_src_file).out $($@_src_file).expected > $($@_src_file).out.diff || true


update-rpt-%: rpt-%
	$(eval $@_src_file := $(patsubst update-rpt-%,%,$@))
	cp $($@_src_file).rpt $($@_src_file).rpt.expected

update-out-%: out-%
	$(eval $@_src_file := $(patsubst update-out-%,%,$@))
	cp $($@_src_file).out $($@_src_file).expected

update-out-p-%: out-p-%
	$(eval $@_src_file := $(patsubst update-out-p-%,%,$@))
	cp $($@_src_file).out $($@_src_file).expected

$(MULTI):
	$(MAKE) -C $@ $(MAKECMDGOALS)

run: $(patsubst %,diff-out-%,$(SINGLES)) $(patsubst %,diff-rpt-%,$(REPORTS)) $(MULTI) \
	 $(patsubst %,diff-out-p-%,$(PURE_KERNEL_LIST))
	cat \
		$(patsubst %,%.out.diff,$(SINGLES)) \
		$(patsubst %,%.out.diff,$(PURE_KERNEL_LIST)) \
		$(patsubst %,%.rpt.diff,$(REPORTS)) \
		$(patsubst %,%/err.log,$(MULTI)) \
		> err.log

update: $(patsubst %,update-out-%,$(SINGLES)) \
$(patsubst %,update-out-p-%,$(PURE_KERNEL_LIST)) \
	$(patsubst %,update-rpt-%,$(REPORTS)) $(MULTI)

clean: $(MULTI)
	rm -f \
		$(patsubst %,%.out,$(SINGLES)) \
		$(patsubst %,%.out.diff,$(SINGLES)) \
		$(patsubst %,%.rpt,$(REPORTS)) \
		$(patsubst %,%.rpt.diff,$(REPORTS)) \
		err.log
	# really annoying byproducts of mars_opt
	rm -f flex*.log history_*.json $(patsubst %.cpp,%.ti,$(SINGLES) $(REPORTS))

# The mars_test test runner expects this target to exist. TODO: Remove this once
# mars_test has been updated.
unit_test: run
diff: run
gen_ref: update
update_ref: update

.PHONY: clean run update diff $(MULTI)
