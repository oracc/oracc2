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
pt=`head -1 01bld/00cat/local-p.tsv`
qt=`head -1 01bld/00cat/local-q.tsv`
xt=`head -1 01bld/00cat/local-x.tsv`

# extract the P for each project and expand the tsv according to the template
for f in ${tdir}/*.P ; do
    if [ -s $f ]; then
	p=`basename $f` .P
	l=${ORACC}/$p/01bld/00cat/local-p.tsv
	t=${tdir}/${p}-p.tsv
	head -1 $l >$t
	grep $l >>$t
	rocox -E -T'"'${pt}'"' $t >>01bld/00cat/outer-p.tsv
	rm -f $t
    fi
done

# extract the Q for each project and expand the tsv according to the template
for f in ${tdir}/*.Q ; do
    if [ -s $f ]; then
	p=`basename $f` .Q
	l=${ORACC}/$p/01bld/00cat/local-q.tsv
	t=${tdir}/${p}-q.tsv
	head -1 $l >$t
	grep $l >>$t
	rocox -E -T'"'${pt}'"' $t >>01bld/00cat/outer-q.tsv
	rm -f $t
    fi
done

# extract the X for each project and expand the tsv according to the template
for f in ${tdir}/*.X ; do
    if [ -s $f ]; then
	p=`basename $f` .X
	l=${ORACC}/$p/01bld/00cat/local-x.tsv
	t=${tdir}/${p}-x.tsv
	head -1 $l >$t
	grep $l >>$t
	rocox -E -T'"'${xt}'"' $t >>01bld/00cat/outer-x.tsv
	rm -f $t
    fi
done
