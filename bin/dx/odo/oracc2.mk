include .deps

project: build.log

check: check.log

build.log: ${DEPS}
	oracc build 2>build.log
	cat build.log

check.log: ${DEPS}
	oracc check 2>check.log
	cat check.log

.deps:
	@makedeps.sh
