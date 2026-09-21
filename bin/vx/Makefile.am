include ../../oracc-am-rules.txt

bin_PROGRAMS=vx
data_DATA=catf-map.tsv

vx_SOURCES=vx.c vx_load.c vx_xtf.c vx_lib.c vx_conllo.c vx_identity.c \
	vx_cun.c vx_tra.c \
	conll.c vx_epsd.c vx_oatf.c \
	vx_catf.c vxc_fnc.c vxcfnctab.c utf2atf.c

# vx_atf.c

vx_LDADD = $(xLDADD) 
vx_DEPENDENCIES = $(xLDADD)
