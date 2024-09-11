#!/bin/sh
grep -F ']PN' $1.ngm | grep -F ']N' | perl -n -e '/\]N[^\]]+$/ && print' | perl -p -e 's/^.*?\]\S+\s+//' | sort -u >$1-tok2.lst
