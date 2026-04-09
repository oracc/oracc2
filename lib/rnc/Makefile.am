include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=librnc.la

rncdata_DATA=*.rnc

librnc_la_SOURCES=asl.c gdl.c xtf.c numsys.c cbd.c cbd_summaries.c inl.c

asl.c: asl.rng grapheme.rng graphmeta.rng words.rng inl.rng

cbd.c: cbd.rng grapheme.rng graphmeta.rng words.rng

cbd_summaries.c: cbd_summaries.rng cbd-lib.rng grapheme.rng graphmeta.rng words.rng

xtf.c: xtf.rng grapheme.rng graphmeta.rng words.rng xcl.rng xcl-lib.rng

