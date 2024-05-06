SUBDIRS=pui ice isp

include ../../oracc-am-rules.txt

AM_CPPFLAGS+=-I/usr/include/libxml2

img_DATA=icons/*.png

bin_PROGRAMS=px

bin_SCRIPTS=pxlangs.sh pxreset.sh pxp3p4.sh \
	pxsortercodes.sh pxentrycounts.sh pxentryindexes.sh

scriptdata_DATA=*.xsl

px_SOURCES=px.c pxargs.c px_help.c px_options.c px_validate.c cgiopts.c \
	pxloadfile.c px_file_copy.c xslt_if.c

px_LDADD=isp/libisp.la pui/libpui.la $(xLDADD) -lxml2 -lxslt -lexslt

css_DATA = *.css

js_DATA = p4.js
