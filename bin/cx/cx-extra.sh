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
	cp $t 01tmp/00cat/cat.d
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
		    (
			cd 01tmp/00cat/cat.d ;
			b=`basename $t .tsv`
			c="$t.tsv"
			if $(grep -q ^P $c); then
			    head -1 $c | sed 's/^id_composite/id_text/' >$cdir/$b-p.tsv
			    grep ^P $c >>$cdir/$b-p.tsv
			elif $(grep -q ^Q $c); then
			    head -1 $c | sed 's/^id_text/id_composite/' >$cdir/$b-q.tsv
			    grep ^Q $c >>$cdir/$b-q.tsv
			elif $(grep -q ^X $c); then
			    head -1 $c | sed 's/^id_composite/id_text/' >$cdir/$b-x.tsv
			    grep ^X $c >>$cdir/$b-x.tsv
			fi
		    )
		else
		    >&2 echo $0: no ID found in first row of $t
		fi
	    fi
	fi
    done
fi

# Now if there are any files with extra data for p/q/x but there is no
# corresponding local-p/q/x, generate the corresponding local file
# with all of the IDs to act as host for rocox -J
for pqx in p q x ; do
    if [ ! -s 01tmp/00cat/local-$pqx.tsv ]; then
	$xtsv="01tmp/00cat/cat.d/*-$pqx.tsv"
	set
	if [ "$1" != "$xtsv" ]; then
	    head -1 $1 >local-$pqx.tsv
	    cut -1 $xtsv | grep -v ^id | sort -u >>local-$pqx.tsv
	fi
    fi
done
