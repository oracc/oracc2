include ../../../oracc-am-rules.txt

bin_PROGRAMS=xsltx

AM_CPPFLAGS+=-I/usr/include/libxml2

xsltx_SOURCES = xsltx.c progtab.c
xsltx_LDADD = $(xLDADD) -lxml2 -lxslt
xsltx_DEPENDENCIES = $(xLDADD)
