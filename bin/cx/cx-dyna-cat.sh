#!/bin/sh
#
# Create 01bld/cat/dynamic-[pqx].tsv from the list given in the
# argument file
#
# The list is cat-qualified, PQX@CATPROJECT. PQX is looked
# for first in CATPROJECT then in the fallback projects.
#
# Fallback projects can be listed in the config item
# 'catalog-master-project' and default to:
#
#	P cdli
#	Q qcat
#	X xcat
#
# Items not found in xcat have basic data generated from the &-line in
# their ATF.
#
echo $0 $*
bin=${ORACC}/bin
list=$1
work=01tmp/`basename $1`
cut -d: -f2 $list | tr @ '\t' >$work
grep ^P $work >$work.P
grep ^Q $work >$work.Q
grep ^X $work >$work.X

project=`oraccopt`

$bin/cx-xcat.sh

pdyna=01bld/cat/dynamic-p.cat
qdyna=01bld/cat/dynamic-q.cat
xdyna=01bld/cat/dynamic-x.cat

rm -f $pdyna $qdyna $xdyna

pmaster=`oraccopt . catalog-master-p`
qmaster=`oraccopt . catalog-master-q`
xmaster=`oraccopt . catalog-master-x`

if [ "${pmaster}" = "" ]; then
    pcat=${ORACC}/cdli/00cat/cdli_catalogue.tsv
else
    pcat=${pmaster}
fi
if [ "${qmaster}" = "" ]; then
    qcat=${ORACC}/qcat/00cat/qcat.tsv
else
    qcat=${qmaster}
fi
if [ "${xmaster}" = "" ]; then
    xcat=01bld/cat/auto-x.tsv
else
    xcat=${xmaster}
fi

cat $work.P | while read IFS=$'\t' id cat ; do
    if [ "${cat}" != "${project}" ]; then
	data=`grep ^$id ${ORACC}${cat}/00cat/*.tsv`
    fi
    if [ "${data}" = "" ]; then
	data=`grep ^$id $pcat`
    fi
    if [ "${data}" = "" ]; then
	echo "$0: no data found for $id in $cat or $pcat"
    else
	echo ${data} >>$pdyna
    fi
done

cat $work.Q | while read IFS=$'\t' id cat ; do
    if [ "${cat}" != "${project}" ]; then
	data=`grep ^$id ${ORACC}${cat}/00cat/*.tsv`
    fi
    if [ "${data}" = "" ]; then
	data=`grep ^$id $qcat`
    fi
    if [ "${data}" = "" ]; then
	echo "$0: no data found for $id in $cat or $qcat"
    else
	echo ${data} >>$qdyna
    fi
done

cat $work.X | while read IFS=$'\t' id cat ; do
    if [ "${cat}" != "${project}" ]; then
	data=`grep ^$id ${ORACC}${cat}/00cat/*.tsv`
    fi
    if [ "${data}" = "" ]; then
	data=`grep ^$id $xcat`
    fi
    if [ "${data}" = "" ]; then
	echo "$0: no data found for $id in $cat or $xcat"
    else
	echo ${data} >>$xdyna
    fi
done
