include ../../../oracc-am-rules.txt

AM_CPPFLAGS+=-I/usr/include/libxml2

noinst_LTLIBRARIES=libpui.la

libpui_la_SOURCES=all.proto \
	p4controls.c p4footer.c p4icons.c p4pager.c p4search.c \
	p4error.c p4html.c p4head.c p4dropdown.xml p4debug.c \
	p4debugdiv.c p4itemglo.c p4itemxtf.c p4select.xsl p4ceselect.xml \
	p4unknown.c p4noxtf.c p4nosrch.c \
	atactions.c puifiles.c pui_at.c pui_output.c pui_123.c

.xml.c:
	./xc.plx $<
.xsl.c:
	./xc.plx $<

CLEANFILES=p4*.c
