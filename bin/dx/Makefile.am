include ../../oracc-am-rules.txt

bin_PROGRAMS=dcx
noinst_SCRIPTS=dx-install.sh
sbin_PROGRAMS=dx

dx_SOURCES=dx.c dx_listen.c dx_accept.c dx_prechecks.c \
	dx_exec.c dx_process.c dx_session.c dx_signal.c

dcx_SOURCES=dcx.c dx_connect.c

install-exec-hook:
	/bin/sh ./dx-install.sh

subdirs=osh odo
