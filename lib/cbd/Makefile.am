include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=libcbd.la

libcbd_la_SOURCES=cbd.tab.c cbd.lex.c cbdyacc.c cbdpos.c cbdrws.c cbdtags.c

cbd.tab.c cbd.tab.h: cbd.y cbd.l

cbd.lex..c: cbd.l cbd.tab.h
	flex -d -B -DYY_NO_INPUT cbd.l
	grep '#define s_' cbd.lex.c >startstates.h
