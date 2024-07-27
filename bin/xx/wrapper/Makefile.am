include ../../../oracc-am-rules.txt

bin_PROGRAMS=xsltx

scriptdata_DATA=p4-htmlview.xsl p4-corpusview-lib.xsl p4-g2-gdl-HTML.xsl p4-g2-xtf-HTML.xsl

noinst_SCRIPTS=wrapper-links.sh

AM_CPPFLAGS+=-I/usr/include/libxml2

xsltx_SOURCES = xsltx.c progtab.c
xsltx_LDADD = $(xLDADD) -lxml2 -lxslt -lexslt
xsltx_DEPENDENCIES = $(xLDADD)

install-exec-hook:
	/bin/bash wrapper-links.sh
