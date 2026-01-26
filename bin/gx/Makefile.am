include ../../oracc-am-rules.txt

bin_PROGRAMS=siglangx sigx gx

gx_SOURCES=common.c gx.c o_glo.c o_jox.c o_jsn.c validate.c iterator.c o_ratts.c
gx_LDADD = $(xLDADD)
gx_DEPENDENCIES = $(gx_LDADD) o_glo_fncs.c o_jox_fncs.c o_jsn_fncs.c validate_fncs.c

siglangx_SOURCES=siglangx.c qpnnames.c
siglangx_LDADD = $(xLDADD)
siglangx_DEPENDENCIES = $(siglangx_LDADD)

sigx_SOURCES=sigx.c ../../lib/liboracc2.la
sigx_LDADD = $(xLDADD)
sigx_DEPENDENCIES = $(sigx_LDADD)
