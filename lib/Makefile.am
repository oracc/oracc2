include ../oracc-am-rules.txt

SUBDIRS=asl atf cat cbd chr collate expat frm gdl gvl inl \
	joxer jsn libstemmer_c lng ofp oid rnc rnv sas sll std sys \
	tis tok tsv xmd xml xpd l3 scan kis

noinst_LTLIBRARIES=liboracc2.la

liboracc2_la_SOURCES=liboracc2.c

liboracc2_la_LIBADD=asl/*.lo atf/*.lo cat/*.lo cbd/*.lo chr/*.lo	\
	collate/*.lo expat/*.lo frm/*.lo gdl/*.lo gvl/*.lo joxer/*.lo	\
	jsn/*.lo lng/*.lo ofp/*.lo oid/*.lo rnc/*.lo rnv/*.lo		\
	sas/*.lo sll/*.lo std/*.lo sys/*.lo tis/*.lo tok/*.lo		\
	tsv/*.lo xmd/*.lo xml/*.lo xpd/*.lo libstemmer_c/*/*.lo		\
	l3/*/*.lo inl/*.lo scan/*.lo
