#!/bin/sh
#
# Generate a .tsv that contains all the data for the catalogue
#

xall=01tmp/00cat/all
mkdir -p $xall
xtsv=01tmp/00cat/cat.d/*.tsv
set $xtsv
if [ "$1" != "$xtsv" ]; then
    ls -1 $xtsv >01tmp/cat-join.lst
    for c in 01tmp/00cat/*.tsv ; do
	b=`basename $c`
	# augment local-p.tsv as local-p.tsv.all and mv it back
	rocox -J 01tmp/cat-join.lst $c >$c.all && mv $c.all $c
    done
fi
