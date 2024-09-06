include ../oracc-am-rules.txt

SUBDIRS=asl atf cat cbd chr collate expat frm gdl gvl \
	joxer jsn libstemmer_c lng oid rnc rnv sll std sys \
	tis tok tsv xmd xml xpd l3

noinst_LTLIBRARIES=liboracc2.la

liboracc2_la_SOURCES=liboracc2.c

liboracc2_la_LIBADD=asl/*.lo atf/*.lo \
	cat/*.lo cbd/*.lo chr/*.lo collate/*.lo expat/*.lo frm/*.lo \
	gdl/*.lo gvl/*.lo joxer/*.lo jsn/*.lo lng/*.lo oid/*.lo \
	rnc/*.lo rnv/*.lo sll/*.lo std/*.lo sys/*.lo tis/*.lo \
	tok/*.lo tsv/*.lo xmd/*.lo xml/*.lo xpd/*.lo libstemmer_c/*/*.lo

# l3/*/*.lo
