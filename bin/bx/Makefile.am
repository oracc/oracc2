include ../../oracc-am-rules.txt

bin_SCRIPTS=bx.sh bxbibkey.sh bxbibres.sh

bin_PROGRAMS=bx
bx_SOURCES=bx.c bx_keys.c bx_bibs.c bbl.tab.c bbl.lex.c bx_bbl.c bx_4ht.c

bx_LDADD = $(xLDADD)
bx_DEPENDENCIES = $(bx_LDADD)

bbl.tab.c bbl.tab.h: bbl.y bbl.l

bbl.lex.c: bbl.l bbl.tab.h
