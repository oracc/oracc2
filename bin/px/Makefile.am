SUBDIRS=iso isp iss pui ice wht

#pxr

include ../../oracc-am-rules.txt

AM_CPPFLAGS+=-I/usr/include/libxml2

img_DATA=icons/*.png

bin_PROGRAMS=px cbditem issx wsigx xfragx

scriptdata_DATA=p4-xmd-div.xsl p4-stats-HTML.xsl p4-proofing.xsl

pxlib=pxargs.c px_options.c px_validate.c cgiopts.c pxloadfile.c \
	px_print_ip.c px_file_copy.c cgi_decode_url.c px_mesg.c \
	lang-3-2.c px_help.c s123.c sigmap.c px_find_file.c

issx_SOURCES=issx.c $(pxlib)
issx_LDADD = isp/libisp.la wht/libwht.la iss/libiss.la iso/libiso.la $(xLDADD)
issx_DEPENDENCIES = $(issx_LDADD)

wsigx_SOURCES=wsigx.c
wsigx_LDADD = $(xLDADD)
wsigx_DEPENDENCIES = $(xLDADD)

xfragx_SOURCES=xfragx.c xftable.c
xfragx_LDADD = $(xLDADD)
xfragx_DEPENDENCIES = $(xLDADD)

bin_SCRIPTS=pxlangs.sh pxreset.sh pxperms.sh pxp3p4.sh pxsrch.sh \
	pxentrycounts.sh pxentryindexes.sh p4-mk-cache.sh \
	pxlocalreset.sh cbd-items.sh pxprecompute.sh pxtsvdbs.sh \
	outline0.sh

px_SOURCES=px.c xslt_if.c wpx_lib.c selecter.c http_accept_language.c px_error_page.c \
	px_images.c $(pxlib)

px_print_ip.c: isp/isp.h mk-isp-print.plx
	./mk-isp-print.plx

px_LDADD=wht/libwht.la pui/libpui.la isp/libisp.la iss/libiss.la iso/libiso.la $(xLDADD) -lxml2 -lxslt -lexslt

# pxr/libpxr.la

cbditem_SOURCES=cbditem.c
cbditem_LDADD=$(xLDADD)

#selecter_SOURCES=selecter.c
#selecter_LDADD=$(xLDADD)

css_DATA = *.css

js_DATA = *.js
