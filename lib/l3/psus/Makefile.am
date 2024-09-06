include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libpsus.la

psus.c: psus.c.in
	$(ORACCIFY) psus.c.in

libpsus_la_SOURCES=psu2.c links_psu.c
