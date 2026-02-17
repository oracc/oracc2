include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=librnc.la

rncdata_DATA=*.rnc

librnc_la_SOURCES=asl.c gdl.c xtf.c numsys.c cbd.c cbd_summaries.c

cbd.c: cbd.rng grapheme.rng graphmeta.rng words.rng

cbd-summaries.c: cbd-summaries.rng cbd-lib.rng grapheme.rng graphmeta.rng words.rng
