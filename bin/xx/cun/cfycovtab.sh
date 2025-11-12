#!/bin/sh
grep -v ^:: $1 | cut -f2- | sort -u | sed 's/^/U+/' >$$.cct
cut -f1 $$.cct | grep -f - $ORACC/osl/02pub/unicode.tsv | cut -f1-2 | sed 's/^U+//' >$$.nam
cut -f2 $$.cct | paste $$.nam - >cfycov.tab
