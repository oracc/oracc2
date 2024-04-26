SUBDIRS=pui ice isp

include ../../oracc-am-rules.txt

bin_PROGRAMS=px

px_SOURCES=px.c

px_LDADD=ccgi/libccgi.la -lexpat $(xLDADD)
