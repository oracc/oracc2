include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=libasl.la

libasl_la_SOURCES=asl.tab.c asl.lex.c aslyacc.c asl.c asl_bld.c asl_listdef.c

asl.tab.c asl.tab.h: asl.y asl.l

asl.lex.c: asl.l asl.tab.h
