include ../oracc-am-rules.txt

SUBDIRS=asl atf cat cbd collate expat frm gdl gvl jsn \
	lng oid rnc rnv sll std sys xml xpd 

noinst_LTLIBRARIES=liboracc2.la

liboracc2_la_SOURCES=liboracc2.c

liboracc2_la_LIBADD=asl/*.lo atf/*.lo \
	cat/*.lo cbd/*.lo collate/*.lo expat/*.lo frm/*.lo \
	gdl/*.lo gvl/*.lo jsn/*.lo lng/*.lo oid/*.lo rnc/*.lo rnv/*.lo \
	sll/*.lo std/*.lo sys/*.lo xml/*.lo xpd/*.lo 
