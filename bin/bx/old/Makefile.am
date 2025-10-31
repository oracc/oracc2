include ../../oracc-am-rules.txt

bin_SCRIPTS=bx.sh bxbibkey.sh bxbibres.sh

bin_PROGRAMS=bx bibx
bx_SOURCES=bx.c bx_keys.c bx_bibs.c bbl.tab.c bbl.lex.c bx_bbl.c bx_4ht.c

bx_LDADD = $(xLDADD)
bx_DEPENDENCIES = $(bx_LDADD)

bbl.tab.c bbl.tab.h: bbl.y bbl.l

bbl.lex.c: bbl.l bbl.tab.h

bibx_SOURCES=bibx.c bib.tab.c bib.lex.c bib_ent.c bib_fld.c bib_xml.c

bibx_LDADD = $(xLDADD)
bibx_DEPENDENCIES = $(bibx_LDADD)

bib.tab.c bib.tab.h: bib.y bib.l

bib.lex.c: bib.l bib.tab.h

