include ../../oracc-am-rules.txt

bin_PROGRAMS=siglangx sigx
noinst_PROGRAMS=gx

gx_SOURCES=common.c gx.c o_glo.c o_xml.c validate.c iterator.c
gx_LDADD = $(xLDADD)
gx_DEPENDENCIES = $(gx_LDADD) o_glo_fncs.c o_xml_fncs.c validate_fncs.c

siglangx_SOURCES=siglangx.c qpnnames.c
siglangx_LDADD = $(xLDADD)
siglangx_DEPENDENCIES = $(siglangx_LDADD)

sigx_SOURCES=sigx.c
sigx_LDADD = $(xLDADD)
sigx_DEPENDENCIES = $(sigx_LDADD)
