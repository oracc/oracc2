include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libpui.la

libpui_la_SOURCES=\
	p4controls.c p4footer.c p4icons.c p4pager.c p4search.c \
	p4error.c p4html.c p4head.c p4dropdown.xml p4debug.c \
	p4debugdiv.c p4itemxtf.c \
	atactions.c puifiles.c pui_at.c pui_output.c

.xml.c:
	./xc.plx $<

CLEANFILES=p4*.c
