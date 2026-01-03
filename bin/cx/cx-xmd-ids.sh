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
    cat 01tmp/00cat/*.tsv | cut -f1 | sort -u | sed "s/^/$project:/" >01tmp/xmdids.lst
fi
