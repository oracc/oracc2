SUBDIRS=pui ice isp

include ../../oracc-am-rules.txt

bin_PROGRAMS=px

px_SOURCES=px.c pxargs.c px_help.c px_options.c px_validate.c

px_LDADD=isp/libisp.la pui/libpui.la $(xLDADD)

css_DATA = p4.css p4error.css

js_DATA = p4.js
