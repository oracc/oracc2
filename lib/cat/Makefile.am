include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=libcat.la

libcat_la_SOURCES = cat.tab.c cat.lex.c catyacc.c cattree.c cat.c

cat.tab.c cat.tab.h: cat.y cat.l

cat.lex.c: cat.l cat.tab.h
