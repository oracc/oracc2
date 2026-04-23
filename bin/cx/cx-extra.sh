#!/bin/sh
#
# Marshall extra inputs for .tsv in 01tmp/00cat
#
# For 00lib/cat.d/*.tsv link to 01tmp/00cat/cat.d/*.tsv
#
# For 00lib/cat.d/*.xml convert 00lib/cat.d/*.xml to 01tmp/00cat/cat.d/*.tsv
#
# If there is no local-*.tsv to correspond to
# 01tmp/00cat/cat.d/*-[pqx].tsv generate a skeleton local-[pqx].tsv
# with just the IDs
#
echo $0 $*
echo mdir=$mdir
bin=${ORACC}/bin
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
	    if [ "$id" != "" ]; then
		id=`head -1 $t | tr '\t' '\n' | grep ^id_ | tr -d '\n'`
		if [ "$id" != "" ]; then		    
		    rocox -z $id $t >$t.z
		    mv $t.z $t
		    (
			cd 01tmp/00cat/cat.d ;
			b=`basename $t .tsv`
			c="$b.tsv"
			if $(grep -q ^P $c); then
			    head -1 $c | sed 's/^id_composite/id_text/' >$b-p.tsv
			    grep ^P $c >>$b-p.tsv
			elif $(grep -q ^Q $c); then
			    head -1 $c | sed 's/^id_text/id_composite/' >$b-q.tsv
			    grep ^Q $c >>$b-q.tsv
			elif $(grep -q ^X $c); then
			    head -1 $c | sed 's/^id_composite/id_text/' >$b-x.tsv
			    grep ^X $c >>$b-x.tsv
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
# corresponding local-p/q/x, generate a suitable extra file with all
# of the IDs to act as host for rocox -J and run the join with output
# in $mdir
for pqx in p q x ; do
    if [ ! -s 01tmp/00cat/local-$pqx.tsv ]; then
	xtsv="01tmp/00cat/cat.d/*-$pqx.tsv"
	set $xtsv
	if [ "$1" != "$xtsv" ]; then
	    extra=extra-$pqx.tsv
	    tsv=01tmp/00cat/extra-$pqx.tsv
	    head -1 $1 | cut -f1 >$tsv
	    cut -f1 $xtsv | grep -v ^id | sort -u >>$tsv
	    ls -1 $xtsv >01tmp/cat-join.lst
	    ${bin}/rocox -J 01tmp/cat-join.lst <01tmp/00cat/$extra >$mdir/$extra
	fi
    fi
done
