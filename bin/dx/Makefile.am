include ../../oracc-am-rules.txt

SUBDIRS=lib dxx osh odo

bin_PROGRAMS=odox
noinst_SCRIPTS=odox-install.sh

odox_SOURCES=odox.c
odox_LDADD=$(xLDADD)

install-exec-hook:
	/bin/sh ./odox-install.sh
