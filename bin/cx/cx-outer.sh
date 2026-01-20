#!/bin/sh
#
# Take a list of IDs and group them by metadata project and
# PQX-type. For each group, use rocox -ET to rewrite them according to
# the template
echo $0 $*
tdir=01tmp/00cat/t
rm -fr ${tdir} ; mkdir -p ${tdir}
list=$1

# split the input list by project
proj=`cut -d@ ${list} -f2 | sort -1 | tr '\n' ' '`
for p in $proj; do
    for pqx in P Q X; do
	grep @$p ${list} | grep ^$pqx >${tdir}/$p.$pqx
    done
done

# set the templates
lp=01bld/cat/local-p.tsv
lq=01bld/cat/local-q.tsv
lx=01bld/cat/local-x.tsv
op=01bld/cat/outer-p.tsv
oq=01bld/cat/outer-q.tsv
ox=01bld/cat/outer-x.tsv

# initialize the outputs
if [ ! -s $op ]; then
    head -1 $lp >$op
fi
if [ ! -s $oq ]; then
    head -1 $lq >$oq
fi
if [ ! -s $ox ]; then
    head -1 $lx >$ox
fi

pt=`head -1 $lp | tr '\t' +`
qt=`head -1 $lq | tr '\t' +`
xt=`head -1 $lx | tr '\t' +`

# extract the P for each project and expand the tsv according to the template
for f in ${tdir}/*.P ; do
    if [ -s $f ]; then
	p=`basename $f` .P
	l=${ORACC}/$p/01bld/cat/local-p.tsv
	t=${tdir}/${p}-p.tsv
	head -1 $l >$t
	grep $l >>$t
	rocox -E -T$pt $t >>$op
	rm -f $t
    fi
done

# extract the Q for each project and expand the tsv according to the template
for f in ${tdir}/*.Q ; do
    if [ -s $f ]; then
	p=`basename $f` .Q
	l=${ORACC}/$p/01bld/cat/local-q.tsv
	t=${tdir}/${p}-q.tsv
	head -1 $l >$t
	grep $l >>$t
	rocox -E -T$pt $t >>$oq
	rm -f $t
    fi
done

# extract the X for each project and expand the tsv according to the template
for f in ${tdir}/*.X ; do
    if [ -s $f ]; then
	p=`basename $f` .X
	l=${ORACC}/$p/01bld/cat/local-x.tsv
	t=${tdir}/${p}-x.tsv
	head -1 $l >$t
	grep $l >>$t
	rocox -E -T$xt $t >>$ox
	rm -f $t
    fi
done
