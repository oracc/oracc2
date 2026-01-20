#!/bin/sh
#
# Determine the fields for each P/Q/X type.
#
# If there is a local-[pqx].tsv take it from there.
#
# Else use cat-master or the fallback catalogue. If a project wants to
# use the fields from another project it can use cat-master to achieve
# that.
#
# For X-ids with no local-x.tsv, use the minimal field set ensured by
# atfdatax.
#

$cdir=01bld/cat
pmaster=`oraccopt . catalog-master-p`
qmaster=`oraccopt . catalog-master-q`
xmaster=`oraccopt . catalog-master-x`
fpmaster=${ORACC}/${xmaster}/01bld/cat/local-p.tsv
fqmaster=${ORACC}/${xmaster}/01bld/cat/local-q.tsv
fxmaster=${ORACC}/${xmaster}/01bld/cat/local-x.tsv

if [ -s $cdir/local-p.tsv ]; then
    head -1 $cdir/local-p.tsv >$cdir/fields.p
elif [ -s ${fpmaster} ]; then
    head -1 ${fpmaster} >$cdir/fields.p
else
    cdli=${ORACC}/cdli/01bld/cat/local-p.tsv
    head -1 ${cdli} >$cdir/fields.p
fi

if [ -s $cdir/local-q.tsv ]; then
    head -1 $cdir/local-q.tsv >$cdir/fields.q
elif [ -s ${fqmaster} ]; then
    head -1 ${fqmaster} >$cdir/fields.q
else
    qcat=${ORACC}/qcat/01bld/cat/local-q.tsv
    head -1 ${qcat} >$cdir/fields.q    
fi

if [ -s $cdir/local-x.tsv ]; then
    head -1 $cdir/local-x.tsv >$cdir/fields.x
elif [ -s ${fxmaster} ]; then
    head -1 ${fxmaster} >$cdir/fields.x
else
    atfdatax -X >$cdir/fields.x
fi
