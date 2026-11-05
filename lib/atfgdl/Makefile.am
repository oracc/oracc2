include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=libatfgdl.la

libatfgdl_la_SOURCES = ag.tab.c ag.lex.c ag.c ag_line.c blocktok.c

ag.tab.c ag.tab.h: atfgdl.y atfgdl.l
	bison -vdt $< >$@

ag.lex.c: atfgdl.l ag.tab.h
	flex -d -B -DYY_NO_INPUT -o $@ $<
	indent $@

blocktok.c: blocktok.g ag.tab.h

atfgdl.l: atfgdl.ll ../gdl/gdl.l atfgdl-l.plx
	./atfgdl-l.plx >$@

atfgdl.y: atfgdl.yy ../gdl/gdl.y atfgdl-y.plx
	./atfgdl-y.plx >$@

