#!/bin/sh
#
# Rewrite the marshalled and sanity checked local cat files into
# 01tmp/00cat/m/local-[pqx].tsv.  From there they get augmented by
# cx-extra.sh into 01bld/cat.
#
echo $0 $*
#echo mdir=$mdir
set $ldir/*
if [ "$1" != "$ldir/*" ]; then
    for c in $* ; do
	if $(grep -q ^P $c); then
	    head -1 $c >$mdir/local-p.tsv
	    grep ^P $c >>$mdir/local-p.tsv
	    if $(grep -q ^X $c); then
		head -1 $c >$mdir/local-x.tsv
		grep ^X $c >>$mdir/local-x.tsv		
	    fi
	elif $(grep -q ^Q $c); then
	    head -1 $c >$mdir/local-q.tsv
	    grep ^Q $c >>$mdir/local-q.tsv
	elif $(grep -q ^X $c); then
	    head -1 $c >$mdir/local-x.tsv
	    grep ^X $c >>$mdir/local-x.tsv
	fi
    done
fi
