include ../../oracc-am-rules.txt

subdirs=ccgi

## AM_CPPFLAGS=-I@top_srcdir@/lib/include -I@top_srcdir@/lib/{std,sys,xml} -Wall -pedantic

cgibin_PROGRAMS=wx

#noinst_SCRIPTS=wx-pub-p4projects.sh
#bin_SCRIPTS=wx-is-p4-project.sh

p4url_LIB=p4url.c qsoption.c urlkeys.c p4url_vec.c p4url_eval.c

#noinst_PROGRAMS=p4url
#p4url_SOURCES=mp4url.c px.c $(p4url_LIB)
#p4url_LDADD=ccgi/libccgi.la $(xLDADD)

# content_test
# content_test_SOURCES=content_test.c content.c runexpat.c
# content_test_LDADD=ccgi/libccgi.la -lexpat

wx_SOURCES=wx.c wx_log.c or_builtins.c or_stable_uris.c \
	selections.c builtins.c or_response.c or_common.c xmlrpc.c \
	or_gdf.c or_pqx.c pqx_map.c pqx_h_tab.c ncname.c p3.c listdir.c \
	sl.c content.c runexpat.c glob.c xff.c oid.c oai.c wx_dbxlib.c px.c \
	p4projects.c cfy.c $(p4url_LIB)

wx_LDADD=ccgi/libccgi.la $(xLDADD)

px.h: px.h.in
	$(ORACCIFY) px.h.in

ccgi/libccgi.la:
	(cd ccgi ; make)

oai.c: oai.c.in
	$(ORACCIFY) oai.c.in

oid.c: oid.c.in
	$(ORACCIFY) oid.c.in

or_builtins.c: or_builtins.c.in
	$(ORACCIFY) or_builtins.c.in

or_common.c: or_common.c.in
	$(ORACCIFY) or_common.c.in

or_gdf.c: or_gdf.c.in
	$(ORACCIFY) or_gdf.c.in

or_pqx.c: or_pqx.c.in
	$(ORACCIFY) or_pqx.c.in

wx.c: wx.c.in ccgi/libccgi.la
	$(ORACCIFY) wx.c.in

or_response.c: or_response.c.in
	$(ORACCIFY) or_response.c.in

or_stable_uris.c: or_stable_uris.c.in
	$(ORACCIFY) or_stable_uris.c.in

p3.c: p3.c.in
	$(ORACCIFY) p3.c.in

pqx_map.c: pqx_map.c.in
	$(ORACCIFY) pqx_map.c.in

sl.c: sl.c.in
	$(ORACCIFY) sl.c.in

xff.c: xff.c.in
	$(ORACCIFY) xff.c.in

xmlrpc.c: xmlrpc.c.in
	$(ORACCIFY) xmlrpc.c.in

EXTRA_DIST=*.h
