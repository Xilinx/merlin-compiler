$(eval cases = $(shell ls | grep vec_add))

test: $(addprefix run-, $(cases))

run-%: clean
	- make -C $*

update: $(addprefix up-, $(cases))

up-%:
	- make -C $* update

clean: $(addprefix clean-, $(cases))
	- rm -rf err.log
	- touch err.log

clean-%:
	- make -C $* clean


