include ../../../oracc-am-rules.txt

bin_PROGRAMS=osh
osh_SOURCES=osh.c osh_options.c osh_project.c osh_commands.c osh_exec.c \
	osh_dcx.c ../dx_connect.c osh_cleanup.c osh_wait.c osh_wrapup.c
osh_LDADD=$(xLDADD) ../dx_connect.o
osh_DEPENDENCIES=$(LDADD)
