include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libxli.la

xli_sux.c: xli_sux.c.in
	$(ORACCIFY) xli_sux.c.in

xli.c: xli_sux.c

libxli_la_SOURCES=xli.c
