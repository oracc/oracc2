include ../../oracc-am-rules.txt

bin_SCRIPTS=bx.sh
bin_PROGRAMS=bx

css_DATA=*.css
data_DATA=bm-jrnser.xml bm-names.xml
scriptdata_DATA=bib-HTML.xsl map-jrnser.xsl

BIBLIB=bib.tab.c bib.lex.c bib_ent.c bib_fld.c bib_out_xml.c bib_out_htm.c \
	bib_names.c bnm_tab.c bib_validate.c bib_sort.c bib_year.c

bx_SOURCES=bx.c bx_cit.c bx_icf.c bx_key.c bx_ref.c bx_lib.c ${BIBLIB}

bx_LDADD = $(xLDADD) $(LD_UNISTRING) -lunistring
bx_DEPENDENCIES = $(xLDADD)

bib.tab.c bib.tab.h: bib.y bib.l

bib.lex.c: bib.l bib.tab.h

# bibx
# bibx_SOURCES=bibx.c ${BIBLIB}
# bibx_LDADD = $(xLDADD)
# bibx_DEPENDENCIES = $(bibx_LDADD)

