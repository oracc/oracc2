SUBDIRS=pui ice isp

include ../../oracc-am-rules.txt

img_DATA=icons/*.png

bin_PROGRAMS=px

bin_SCRIPTS=pxlangs.sh pxreset.sh

px_SOURCES=px.c pxargs.c px_help.c px_options.c px_validate.c cgiopts.c pxloadfile.c

px_LDADD=isp/libisp.la pui/libpui.la $(xLDADD)

css_DATA = p4.css p4error.css p4debug.css

js_DATA = p4.js
