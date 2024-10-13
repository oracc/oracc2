#!/bin/sh
#
# remove OIDs that are @fake from the sorted OID list coming in via stdin
#
grep ';fake' $ORACC/osl/02pub/sl/sl.tsv | cut -d';' | sort -u >fakes
comm fakes -
