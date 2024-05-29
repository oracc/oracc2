include ../../oracc-am-rules.txt

bin_PROGRAMS=lx
bin_SCRIPTS=lx-umbrella.sh lx-atf.sh

scriptdata_DATA=

lx_SOURCES=lx.c lx_load.c lx_loadfile.c lx_parse.c lx_item.c lx_output.c lx_sort.c
lx_LDADD = $(xLDADD)
lx_DEPENDENCIES = all.proto $(lx_LDADD)
