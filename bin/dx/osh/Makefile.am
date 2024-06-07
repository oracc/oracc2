include ../../../oracc-am-rules.txt
override xLDADD=../../../lib/liboracc2.la
AM_CPPFLAGS+=-I../lib
bin_PROGRAMS=osh
sbin_SCRIPTS=wosh
osh_SOURCES=osh.c osh_options.c osh_project.c osh_commands.c osh_exec.c \
	osh_dcx.c osh_cleanup.c osh_wait.c osh_wrapup.c osh_log.c
osh_LDADD=$(xLDADD) ../lib/libdx.la 
# The ../dcx dependency ensures ../dx_connect.o is built
osh_DEPENDENCIES=../lib/libdx.la $(LDADD)

../dc:
	(cd .. ; make)
