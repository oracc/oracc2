#!/bin/sh
#
# Marshall extra inputs for .tsv in 01tmp/00cat
#
# For 00lib/cat.d/*.tsv link to 01tmp/00cat/cat.d/*.tsv
#
# For 00lib/cat.d/*.xml convert 00lib/cat.d/*.xml to 01tmp/00cat/cat.d/*.tsv
#
echo $0 $*

rm -fr 01tmp/00cat/cat.d ; mkdir -p 01tmp/00cat/cat.d
if [ ! -d 01tmp/00cat/cat.d ]; then
    echo $0: failed to make 01tmp/00cat/cat.d. Stop.
    exit 1
fi

set 00lib/cat.d/*.tsv
if [ "$1" != "00lib/cat.d/*.tsv" ]; then
    for t in $* ; do
	cp $t 01tmp/00cat/
    done
fi

set 00lib/cat.d/*.xml
if [ "$1" != "00lib/cat.d/*.xml" ]; then
    for x in $* ; do
	t=`basename $x .xml`.tsv 
	xml2tsv $x >01tmp/00cat/cat.d/$t
    done
fi

set 01tmp/00cat/cat.d/*.tsv
if [ "$1" != "01tmp/00cat/cat.d/*.tsv" ]; then
    for t in $* ; do
	if [ -s $t ]; then
	    id=`head -1 $t | grep ^id_`
	    if [ "${id}" = "" ]; then
		id=`head -1 $t | tr '\t' '\n' | grep ^id_ | tr -d '\n'`
		if [ "${id}" != "" ]; then
		    rocox -z ${id} $t >$t.z
		    mv $t.z $t
		else
		    >&2 echo $0: no ID found in first row of $t
		fi
	    fi
	fi
    done
fi
