#!/bin/sh
set 01bld/*/summaries.xml
if [ "$1" != "01bld/*/summaries.xml" ]; then
    echo $0: indexing $*
    for a in $* ; do
	d=`dirname $a`
	g=`basename $d`
	cbditem $a | tee 02pub/cbd/$g/etm.tsv | dbix -d 02pub/cbd/$g -n etm -s
	cp -a 02www/cbd/$g/$g.map 02pub/cbd/$g/map.tsv
	dbix -d 02pub/cbd/$g -n map 02pub/cbd/$g/map.tsv
	chmod -R o+r 02pub/cbd/$g
    done
    rm -f 02pub/cbd//qpn-x-*/etm.tab
    oxidtab=02pub/cbd/oxid.tsv
    rm -f $oxidtab
    set 02pub/cbd/*/etm.tsv
    if [ "$1" != "02pub/cbd/*/etm.tab" ]; then
	for a in $* ; do
	    g=`basename $a .etm`
	    cut -f1 $a | sed "s/$/	$g/" >>$oxidtab
	done
	dbix -d 02pub/cbd -n oxid $oxidtab
	chmod o+r 02pub/cbd/*
    fi
fi
