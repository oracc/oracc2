include ../../../oracc-am-rules.txt

bin_PROGRAMS=ispx

ispx_SOURCES=all.proto ispx.c isp_init.c isp_options.c isp_validate.c \
	ispargs.c isp_help.c isp_show.c isp_tmp.c isp_list.c isp_cache.c \
	isp_tis.c isp_xis.c filecopy.c

ispx_LDADD = $(xLDADD)
ispx_DEPENDENCIES = $(ispx_LDADD)
