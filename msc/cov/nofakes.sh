#!/bin/sh
#
# remove OIDs that are @fake from the sorted OID list coming in via stdin
#
make -s -f $ORACC/lib/data/Makefile.csl fakes
comm -13 fakes -
