include .deps

.DELETE_ON_ERROR: .build .check

project: .build

check: .check

.build: ${DEPS}
	oracc build
	touch $@

.check: ${DEPS}
	oracc check
	touch $@

clean:
	rm -f .build .check 01bld/buildstamp

.deps:
	@makedeps.sh
