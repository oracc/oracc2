include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=libjsn.la

libjsn_la_SOURCES = jsn.tab.c jsn.lex.c jsn_writer.c jsonify.c

jsn.tab.c jsn.tab.h: jsn.y jsn.l

jsn.lex.c: jsn.l jsn.tab.h
