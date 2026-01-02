#!/bin/sh
#
# Marshall inputs for .tsv in 01tmp/00cat
#
# For 00cat/*.tsv link to 01tmp/00cat/*.tsv
#
# For 00cat/*.xml convert 00cat/*.xml to 01tmp/00cat/*.tsv
#
echo $0 $*
bin="$ORACC/bin"
projtype=`oraccopt . type`

rm -fr 01tmp/00cat ; mkdir -p 01tmp/00cat
if [ ! -d 01tmp/00cat ]; then
    echo $0: failed to make 01tmp/00cat. Stop.
    exit 1
fi

set 00cat/*.tsv
if [ "$1" != "00cat/*.tsv" ]; then
    for t in $* ; do
	cp $t 01tmp/00cat/
    done
fi

set 00cat/*.xml
if [ "$1" != "00cat/*.xml" ]; then
    for x in $* ; do
	t=`basename $x .xml`.tsv 
	xml2tsv $t >01tmp/00cat/$t
    done
fi

set 01tmp/00cat/*.tsv
if [ "$1" != "01tmp/00cat/*.tsv" ]; then
    for t in $* ; do
	id=`head -1 $t | grep ^id_`
	if [ "${id}" = "" ]; then
	    id=`head -1 $t | tr '\t' '\n' | grep ^id_ | tr -d '\n'`
	    rocox -z $id $t >$t.z
	    mv $t.z $t
	fi
    done
fi
