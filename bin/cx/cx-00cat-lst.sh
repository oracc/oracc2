#!/bin/sh
#
# cx-marshall.sh should be run before this script.
#
# Replacement for xmd-ids.plx; much simpler now because 01tmp/00cat
# contains all the relevant .tsv with the id_text|composite in col 0.
#
# This implementation is to replace the line:
#
# 	xmd-ids.plx >01tmp/xmdids.lst
#
# in lx-inputs.sh, so it is only intended to work with odo-corpus.sh
# not o2-corpus.sh.
#
echo $0 $*
project=`oraccopt`
if [ "${project}" = "" ]; then
    echo $0: must run in a project directory. Stop.
    exit 1
else
    c=01tmp/00cat/l/*.tsv
    set $c
    if [ "$1" != "$c" ]; then
	cat $* | cut -f1 | cut -d: -f2 | grep '^[PQX]' | sort >01bld/lists/00cat.lst
	sort -u 01bld/lists/00cat.lst >01tmp/00cat.lst
	if [ $(diff -q 01bld/lists/00cat.lst 01tmp/00cat.lst >/dev/null 2>&1) ]; then
	    echo $0: duplicate entries in catalogue:
	    diff 01tmp/00cat.lst 01bld/lists/00cat.lst
	    exit 1
	else
	    exit 0
	fi
    else
	exit 0
    fi
fi
