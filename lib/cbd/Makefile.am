include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=libcbd.la

libcbd_la_SOURCES=cbd.tab.c cbd.lex.c cbdyacc.c cbdpos.c cbdrws.c cbdtags.c \
	cbd_cofs.c cbd_psus.c cbd_no_fo_ba.c cbd_lang.c cbd_cgp.c cbd_key.c \
	cbd_kis.c cbd_form_walk.c cbd_merge.c cbd_oid.c

cbd.tab.c cbd.tab.h: cbd.y cbd.l

cbd.lex..c: cbd.l cbd.tab.h
	flex -d -B -DYY_NO_INPUT cbd.l
	grep '#define s_' cbd.lex.c >startstates.h
