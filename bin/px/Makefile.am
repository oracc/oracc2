SUBDIRS=isp pui ice

include ../../oracc-am-rules.txt

AM_CPPFLAGS+=-I/usr/include/libxml2

img_DATA=icons/*.png

bin_PROGRAMS=px cbditem

#selecter

bin_SCRIPTS=pxlangs.sh pxreset.sh pxperms.sh pxp3p4.sh pxsrch.sh \
	pxentrycounts.sh pxentryindexes.sh p4-mk-cache.sh \
	pxlocalreset.sh cbd-items.sh

scriptdata_DATA=*.xsl

px_SOURCES=px.c pxargs.c px_help.c px_options.c px_validate.c	\
	cgiopts.c pxloadfile.c px_file_copy.c xslt_if.c		\
	cgi_decode_url.c px_mesg.c wpx_lib.c px_return.c selecter.c \
	px_print_ip.c http_accept_language.c lang-3-2.c

px_print_ip.c: isp/isp.h mk-isp-print.plx
	./mk-isp-print.plx

px_LDADD=isp/libisp.la pui/libpui.la isp/libisp.la $(xLDADD) -lxml2 -lxslt -lexslt

cbditem_SOURCES=cbditem.c
cbditem_LDADD=$(xLDADD)

#selecter_SOURCES=selecter.c
#selecter_LDADD=$(xLDADD)

css_DATA = *.css

js_DATA = *.js
