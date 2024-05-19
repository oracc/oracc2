include ../../../../oracc-am-rules.txt

AM_CPPFLAGS+=-I../lib -I@top_srcdir@/bin/qx/lib

cxLDADD=@top_srcdir@/bin/qx/lib/libqx.la $(xLDADD)

bin_PROGRAMS=cex cextfx

cex_SOURCES = ce2.c ce_xmd2.c ce_gdf.c ce_h.c
cex_LDADD = $(cxLDADD)
cex_DEPENDENCIES = $(cxLDADD)

cextfx_SOURCES = ce_xtf.c ce_h.c
cextfx_LDADD = $(cxLDADD)
cextfx_DEPENDENCIES = $(cxLDADD)
