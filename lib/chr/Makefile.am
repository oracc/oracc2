include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=libchr.la

libchr_la_SOURCES=charsets.c subdig.c

charsets.c: charsets.txt charsets.plx
	perl ./charsets.plx >charsets.c
