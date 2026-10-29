include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=libgdl.la

libgdl_la_SOURCES = gdl.tab.c gdl.lex.c gdlparse.c gdlsig.c gdlyacc.c gdl_one_off.c \
	gdl.c gdl_balance.c gdl_legacy.c gdl_mod.c gdlstate.c \
	gdl_gloss.c gdlstream.c gdl_meta.c gdl_group.c \
	gsort.c gt.c \
	gdl_render.c gr_wf.c gr_vx.c scripttok.c \
	gdl_rnv.c gdl_jox.c gdlxml.c

gdl.tab.c gdl.tab.h: gdl.y gdl.l

gdl.lex.c: gdl.l gdl.tab.h
	flex -d -B -DYY_NO_INPUT -o $@ $<
	indent $@

scripttok.c: scripttok.g lex_st_defs.h

lex_st_defs.h: gdl.lex.c
	grep '^#define [AN] ' gdl.lex.c >$@
