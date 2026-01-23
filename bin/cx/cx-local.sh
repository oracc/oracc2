#!/bin/sh
#
# Rewrite the marshalled and sanity checked local cat files into
# 01tmp/00cat/local-[pqx].tsv.  From there they get augmented by
# cx-extra.sh into 01bld/cat.
#
#set -x
set $ldir/*
if [ "$1" != "${ldir}" ]; then
    for c in $* ; do
	if $(grep -q ^P $c); then
	    head -1 $c >$cdir/local-p.tsv
	    grep ^P $c >>$cdir/local-p.tsv
	    if $(grep -q ^X $c); then
		head -1 $c >$cdir/local-x.tsv
		grep ^X $c >>$cdir/local-x.tsv		
	    fi
	elif $(grep -q ^Q $c); then
	    head -1 $c >$cdir/local-q.tsv
	    grep ^Q $c >>$cdir/local-q.tsv
	elif $(grep -q ^X $c); then
	    head -1 $c >$cdir/local-x.tsv
	    grep ^X $c >>$cdir/local-x.tsv
	fi
    done
fi
