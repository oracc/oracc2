SUBDIRS=pui ice isp

include ../../oracc-am-rules.txt

AM_CPPFLAGS+=-I/usr/include/libxml2

img_DATA=icons/*.png

bin_PROGRAMS=px

#selecter

bin_SCRIPTS=pxlangs.sh pxreset.sh pxperms.sh pxp3p4.sh pxsrch.sh \
	pxsortercodes.sh pxentrycounts.sh pxentryindexes.sh p4-mk-cache.sh

scriptdata_DATA=*.xsl

px_SOURCES=px.c pxargs.c px_help.c px_options.c px_validate.c	\
	cgiopts.c pxloadfile.c px_file_copy.c xslt_if.c		\
	cgi_decode_url.c px_mesg.c wpx_lib.c px_return.c selecter.c \
	px_print_ip.c

px_print_ip.c: isp/isp.h mk-isp-print.plx
	./mk-isp-print.plx

px_LDADD=isp/libisp.la pui/libpui.la isp/libisp.la $(xLDADD) -lxml2 -lxslt -lexslt

#selecter_SOURCES=selecter.c
#selecter_LDADD=$(xLDADD)

css_DATA = *.css

js_DATA = p4.js
