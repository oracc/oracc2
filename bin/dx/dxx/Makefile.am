include ../../../oracc-am-rules.txt

bin_PROGRAMS=dcx
sbin_PROGRAMS=dx
sbin_SCRIPTS=dx-start.sh dx-stop.sh
noinst_SCRIPTS=dx-install.sh

dx_SOURCES=dx.c
dx_LDADD=$(xLDADD) ../lib/libdx.la 

dcx_SOURCES=dcx.c
dcx_LDADD=$(xLDADD) ../lib/libdx.la 

install-exec-hook:
	/bin/sh ./dx-install.sh

