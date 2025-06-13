include ../../../oracc-am-rules.txt

AM_CPPFLAGS+=-I/usr/include/libxml2

noinst_LTLIBRARIES=libisp.la

bin_SCRIPTS=ispcfy.sh ispdiv.sh ispent.sh ispimg.sh ispimg-curl.sh \
	ispxtf.sh ispxtl.sh ispxsl.sh

scriptdata_DATA=isp-ce-HTML.xsl isp-xtl2lst.xsl

libisp_la_SOURCES=all.proto isp_init.c nth_line.c isp_glos.c	\
	isp_show.c isp_tmp.c isp_list.c isp_cache.c isp_xmd.c	\
	isp_tis.c isp_xis.c isp_sort.c isp_page.c isp_item.c	\
	isp_srch.c isp_hilited.c isp_config.c isp_dbx.c		\
	isp_xtl_list.c isp_htmd.c isp_adhoc.c isp_srch_tmp.c	\
	dotless.c
