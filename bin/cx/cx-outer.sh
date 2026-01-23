#!/bin/sh
#
# Take a list of IDs and group them by metadata project and
# PQX-type. For each group, use rocox -ET to rewrite them according to
# the template
#
#set -x
echo $0 $*
tdir=01tmp/00cat/t
rm -fr ${tdir} ; mkdir -p ${tdir}
list=$1

if [ "$list" = "" ]; then
    echo $0: must give list on command line. Stop.
    exit 1
fi

function outer {
    pqx=$1
    shift
    PQX=`/bin/echo -n $pqx | tr pqx PQX`
    echo pqx=$pqx PQX=$PQX rest=$*
    for f in $* ; do
	if [ -s $f ]; then
	    F=$f
	    break;
	fi
    done
    if [ "$F" != "" ]; then
	if [ "${list}" != "01bld/lists/proxy-cat.lst" ]; then
	    omaster=`oraccopt . catalog-master-${pqx}`
	    if [ "${omaster}" = "" ]; then
		if [ "${pqx}" = "p" ]; then
		    ocat="cdli"
		elif [ "${pqx}" = "q" ]; then
		    ocat="qcat"
		else
		    ocat="auto"
		fi
	    else
		ocat=${omaster}
	    fi
	fi
	ltsv=01bld/cat/local-${pqx}.tsv
	otsv=01bld/cat/outer-${pqx}.tsv
	# set $otsv line one to a reasonable output template
	if [ -s $ltsv ]; then
	    head -1 $ltsv >$otsv
	else
	    if [ "${ocat}" = "auto" ]; then
		head -1 01tmp/00cat/auto-x.tsv >$otsv
	    elif [ "${ocat}" != "" ]; then
		head -1 ${ORACC}/$qcat/01bld/cat/local-${pqx}.tsv >$otsv
	    else
		ph=`basename $F .${PQX}`
		p=`/bin/echo -n $ph | tr - /`
		head -1 ${ORACC}/$p/01bld/cat/local-${pqx}.tsv >$otsv
	    fi
	fi
	for f in $*; do
	    if [ -s $f ]; then
		if [ "${ocat}" = "auto" ]; then
		    l=01tmp/00cat/auto-x.tsv
		elif [ "${ocat}" != "" ]; then
		    l=${ORACC}/$qcat/01bld/cat/local-${pqx}.tsv
		else
		    ph=`basename $f .${PQX}`
		    p=`/bin/echo -n $ph | tr - /`
		    l=${ORACC}/$p/01bld/cat/local-${pqx}.tsv
		fi
		if [ -s $l ]; then
		    t=${tdir}/${p}-${pqx}.tsv
		    head -1 $l >$t
		    grep ^${PQX} $l >>$t
		    T=`head -1 $otsv | tr '\t' +`
		    rocox -EF -T$T $t >>$otsv
		    rm -f $t
		fi
	    fi
	done
    fi
}

# split the input list by project
proj=`cut -d@ ${list} -f2 | sort -u | tr '\n' ' '`
for p in $proj; do
    ph=`/bin/echo -n $p | tr / -`
    for pqx in P Q X; do
	grep @$p ${list} | grep :$pqx >${tdir}/$ph.$pqx
    done
done

set ${tdir}/*.P
if [ "$1" != "${tdir}/*.P" ]; then
    outer p $*
fi

set ${tdir}/*.Q
if [ "$1" != "${tdir}/*.Q" ]; then
    outer q $*
fi

set ${tdir}/*.X
if [ "$1" != "${tdir}/*.X" ]; then
    outer x $*
fi
