include ../../oracc-am-rules.txt

bin_PROGRAMS=lx
bin_SCRIPTS=lx-lists.sh lx-umbrella.sh lx-atf.sh lx-inputs.sh

scriptdata_DATA=

lx_SOURCES=lx.c lx_load.c lx_loadfile.c lx_parse.c lx_item.c \
	lx_set_args.c lx_set_ops.c lx_output.c lx_sort.c lx_finish.c

lx_LDADD = $(xLDADD)
lx_DEPENDENCIES = all.proto $(lx_LDADD)
