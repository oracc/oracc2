include ../../../../oracc-am-rules.txt
AM_CPPFLAGS+=-I@top_srcdir@/bin/qx/lib

bin_PROGRAMS=wmx wmapx wmdbgx

wmx_SOURCES = wm.c wm_lib.c
wmx_LDADD = $(xLDADD)
wmx_DEPENDENCIES = $(xLDADD)

wmapx_SOURCES = wmapper.c wm_lib.c
wmapx_LDADD = $(xLDADD)
wmapx_DEPENDENCIES = $(xLDADD)

wmdbgx_SOURCES = wmdebug.c wm_lib.c
wmdbgx_LDADD = $(xLDADD)
wmdbgx_DEPENDENCIES = $(xLDADD)
