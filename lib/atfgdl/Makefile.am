include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=libatfgdl.la

libatfgdl_la_SOURCES = ag.c ag.tab.c ag.lex.c ag_line.c blocktok.c

ag.tab.c ag.tab.h: atfgdl.y atfgdl.l
	bison -vdt $< >$@

ag.lex.c: atfgdl.l ag.tab.h
	flex -d -B -DYY_NO_INPUT -o $@ $<
	indent $@

blocktok.c: blocktok.g ag.tab.h

# atfgdl.l: ../atf/atf.l ../gdl/gdl.l atfgdl-l.plx l-prologue.c l-funcs.c
# 	./atfgdl-l.plx >$@

# atfgdl.y: ../atf/atf.y ../gdl/gdl.y atfgdl-y.plx y-funcs.c
# 	./atfgdl-y.plx >$@
