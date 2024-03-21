include ../../../oracc-am-rules.txt

bin_PROGRAMS=ispx ispsortx ispox

#ispmpx

bin_SCRIPTS=ispdiv.sh

scriptdata_DATA=isp-ce-HTML.xsl

iss_sources = iss.c iss_page.c iss_sortinfo.c iss_sicache.c \
	iss_redblack.c iss_sort.c isp_outline.c iss_pmp.c

#iss_outline.c

ispx_SOURCES=all.proto ispx.c isp_init.c isp_options.c isp_validate.c \
	ispargs.c isp_help.c isp_show.c isp_tmp.c isp_list.c isp_cache.c \
	isp_tis.c isp_xis.c isp_sort.c isp_page.c filecopy.c nth_line.c \
	isp_p3.c idactions.c isp_ui.c \
	$(iss_sources)

ispx_LDADD = $(xLDADD)
ispx_DEPENDENCIES = $(ispx_LDADD)

ispsortx_SOURCES=ispsortx.c $(iss_sources)
ispsortx_LDADD = $(xLDADD)
ispsortx_DEPENDENCIES = $(ispsortx_LDADD)

PSOURCES=ispx.c isp_init.c isp_options.c isp_validate.c \
	ispargs.c isp_help.c isp_show.c isp_tmp.c isp_list.c isp_cache.c \
	isp_tis.c isp_xis.c isp_sort.c isp_page.c filecopy.c nth_line.c \
	isp_p3.c idactions.c iss.c iss_page.c iss_sortinfo.c iss_sicache.c \
	iss_redblack.c iss_sort.c iss_outline.c isp_ui.c isp_outline.c

x.proto: $(PSOURCES)
	cproto -e $(AM_CPPFLAGS) $(PSOURCES) >$@
	mv x.proto all.proto

ispox_SOURCES=ispox.c
ispox_LDADD = $(xLDADD)
ispox_DEPENDENCIES = $(xLDADD)

# ispmpx_SOURCES=isp_pmp.c
# ispmpx_LDADD = $(xLDADD)
# ispmpx_DEPENDENCIES = $(xLDADD)

