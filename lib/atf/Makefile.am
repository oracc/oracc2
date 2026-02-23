include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=libatf.la

libatf_la_SOURCES = atf.tab.c atf.lex.c atfyacc.c atftree.c atf.c \
	atf_bld.c nonxtok.c atf_nonx.c blocktok.c atf_lang.c atf_key.c \
	atf_note.c atf_block.c

## block.c

atf.tab.c atf.tab.h: atf.y atf.l

atf.lex.c: atf.l atf.tab.h
