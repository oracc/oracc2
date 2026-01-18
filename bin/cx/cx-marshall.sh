#!/bin/sh
#
# Marshall inputs from 00cat in 01tmp/00cat/local-[pqx].tsv
#
# For 00cat/*.tsv cp to 01tmp/00cat/l/*.tsv
#
# For 00cat/*.xml convert 00cat/*.xml to 01tmp/00cat/l/*.tsv
#
# Then rotate all .tsv to ensure ID is at start and split into
# separate files
#
echo $0 $*
bin="$ORACC/bin"
projtype=`oraccopt . type`

ldir=01tmp/00cat/l

rm -fr $ldir ; mkdir -p $ldir
if [ ! -d $ldir ]; then
    echo $0: failed to make $ldir. Stop.
    exit 1
fi

set 00cat/*.tsv
if [ "$1" != "00cat/*.tsv" ]; then
    for t in $* ; do
	cp $t $ldir
    done
fi

set 00cat/*.xml
if [ "$1" != "00cat/*.xml" ]; then
    for x in $* ; do
	t=`basename $x .xml`.tsv 
	xml2tsv $t >$ldir/$t
    done
fi

set $ldir/*.tsv
if [ "$1" != "$ldir/*.tsv" ]; then
    for t in $* ; do
	id=`head -1 $t | grep ^id_`
	if [ "${id}" = "" ]; then
	    id=`head -1 $t | tr '\t' '\n' | grep ^id_ | tr -d '\n'`
	    rocox -z $id $t >$t.z
	    mv $t.z $t
	fi
    done
fi

cx-sanity.sh
if [ $? -ne 0]; then
    echo $0: failed catalague sanity checks. Stop.
    exit 1
fi

cx-local.sh $ldir/*.tsv

cx-fields.sh
