.PHONY: test
.PHONY: update

test:
	-rm err.log
	make -C metadata test
	diff metadata/log ./ref/log >err.log

update:
	merlin_create_project run -s src
	merlin_flow run
	cp -r run/implement/metadata metadata
	make -C metadata test
	cp metadata/log ./ref

