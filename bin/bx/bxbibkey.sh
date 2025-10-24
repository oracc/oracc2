#!/bin/sh
if [ "$1" == "" ]; then
    echo $0: must give output file name on command line. Stop.
    exit 1
fi
exec 3>$1
d=${ORACC}/lib/bib
cd $d
grep ^@ *.bib | cut -d'{' -f2 | tr -d , | sort -u >&3
