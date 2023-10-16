include ../../oracc-am-rules.txt

bin_PROGRAMS=jx

bin_SCRIPTS=jx.sh

scriptdata_DATA=jx-uptag.xsl

jx.tab.c jx.tab.h: jx.y jx.l

jx.lex.c: jx.l jx.tab.h

jx_SOURCES=jx.tab.c jx.lex.c jx.c
