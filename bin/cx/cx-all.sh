#!/bin/sh
#
# Generate a .tsv that contains all the data for the catalogue
#
# This script is called as part of cx-marshall.sh where we are only
# working on local-[pqx].tsv
bin=${ORACC}/bin
xtsv=01tmp/00cat/cat.d/*.tsv
${bin}/cx-clean.sh $xtsv
set $xtsv
if [ "$1" != "$xtsv" ]; then
    ls -1 $xtsv >01tmp/cat-join.lst
    set 01tmp/00cat/*.tsv
    if [ "$*" != "01tmp/00cat/*.tsv" ]; then
	for c in $* ; do
	    b=`basename $c`
	    echo c=$c b=$b
	    # augment local-[pqx].tsv into 01bld/cat/local-[pqx].tsv
	    ${bin}/rocox -J 01tmp/cat-join.lst $c >01bld/cat/$b
	done
    fi
else
    cp -v 01tmp/00cat/local-[pqx].tsv 01bld/cat
fi
