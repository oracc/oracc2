include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=libatf.la

libatf_la_SOURCES = atf.tab.c atf.lex.c atfyacc.c atftree.c atf.c \
	nonxtok.c nonx.c blocktok.c block.c

atf.tab.c atf.tab.h: atf.y atf.l

atf.lex.c: atf.l atf.tab.h
