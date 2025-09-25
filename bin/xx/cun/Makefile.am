include ../../../oracc-am-rules.txt

bin_PROGRAMS=cfy
bin_SCRIPTS=cfycov.sh cfylgs.sh
scriptdata_DATA=*.xsl

# cunx
# cunx_SOURCES = cunx.c fileargs.c perfnt.c render.c
# cunx_LDADD = $(xLDADD)
# cunx_DEPENDENCIES = $(xLDADD)

cfy_SOURCES = cfy.c cfy_class.c cfy_reader.c cfy_render.c \
	cfy_out_html.c cfy_out_xml.c fileargs.c perfnt.c \
	cfy_breakage.c cfy_liga.c cfy.tab.c cfy.lex.c \
	cfy_config.c cfy_iterator.c cfy_subs.c subtok.c \
	cfy_uni.c fontname.c cfy_lgs.c cfy_justify.c

cfy_LDADD = $(xLDADD)
cfy_DEPENDENCIES = $(xLDADD)

cfy.tab.c cfy.tab.h: cfy.y cfy.l

cfy.lex.c: cfy.l cfy.tab.h

fontname.g: ../../../msc/fonts/*.def
	./fontname.sh $^
