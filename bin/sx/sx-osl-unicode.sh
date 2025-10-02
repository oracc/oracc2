#!/bin/sh
# Convenience script to update 02pub/unicode.tsv properly before
# rebuilding msc/fonts
cd ${ORACC}/osl
sx -u 00lib/osl.asl
cut -f 2-4,6 unicode.tsv | grep '	o[0-9]\+	' >02pub/unicode.tsv
rm -f unicode.tsv
