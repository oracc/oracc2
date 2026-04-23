#!/bin/sh
#
# Generate a .tsv that contains all the data for the catalogue
#
# This script is called as part of cx-marshall.sh where we are only
# working on local-[pqx].tsv
bin=${ORACC}/bin

for pqx in p q x ; do
    xtsv=01tmp/00cat/cat.d/*-$pqx.tsv
    set $xtsv
    if [ "$1" != "$xtsv" ]; then
	ls -1 $xtsv >01tmp/cat-join.lst
	${bin}/rocox -J 01tmp/cat-join.lst $c >01bld/cat/local-$pqx.tsv
    else
	cp -v 01tmp/00cat/local-$pqx.tsv 01bld/cat
    fi
fi
