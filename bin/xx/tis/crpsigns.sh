#!/bin/sh
#
# Generate a list of SL OIDs that is used in a set of XTF files.
#
p=$1
if [ "$p" == "" ]; then
    echo $0: must give project on command line. Stop.
    exit 1
fi
s=${ORACC}/$p/01bld/lists/have-xtf.lst
if [ ! -r $s ]; then
    echo $0: no such file $s. Stop.
    exit 1
fi
#cat $s | tokx | grep ^g | cut -d. -f1 | cut -f2 | concord -nst | sort -k2nr
make -f $ORACC/lib/data/Makefile.csl need

