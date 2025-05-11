include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=libgdl.la

libgdl_la_SOURCES = gdl.tab.c gdl.lex.c gdlyacc.c gdl.c gdlxml.c \
	gdl_balance.c gdl_legacy.c gdl_lexfld.c gdl_mod.c gdlstate.c \
	gdl_word.c gdlsig.c gdl_rnv.c gdl_one_off.c gdl_jox.c \
	gdlseq.c gsort.c

gdl.tab.c gdl.tab.h: gdl.y gdl.l

gdl.lex.c: gdl.l gdl.tab.h
