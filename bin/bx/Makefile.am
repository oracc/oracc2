include ../../oracc-am-rules.txt

bin_SCRIPTS=bx.sh
bin_PROGRAMS=bx bibx

data_DATA=bm-jrnser.xml bm-names.xml

BIBLIB=bib.tab.c bib.lex.c bib_ent.c bib_fld.c bib_xml.c bib_names.c bnm_tab.c

bx_SOURCES=bx.c bx_cit.c bx_icf.c bx_key.c bx_ref.c bx_lib.c ${BIBLIB}

bx_LDADD = $(xLDADD)
bx_DEPENDENCIES = $(bx_LDADD)

bibx_SOURCES=bibx.c ${BIBLIB}

bibx_LDADD = $(xLDADD)
bibx_DEPENDENCIES = $(bibx_LDADD)

bib.tab.c bib.tab.h: bib.y bib.l

bib.lex.c: bib.l bib.tab.h
