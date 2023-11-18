include ../../oracc-am-rules.txt

bin_PROGRAMS=dcx
noinst_SCRIPTS=dx-install.sh
sbin_PROGRAMS=dx

dx_SOURCES=dx.c dx_exec.c dx_process.c dx_session.c

install-exec-hook:
	/bin/sh ./dx-install.sh
