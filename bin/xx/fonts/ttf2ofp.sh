#!/bin/sh
bin=`dirname $0`
ff=$1
if [ "$ff" == "" ]; then
    echo $0: must give basename of .ttf file on command line. Stop.
    exit 1;
fi
f=`basename $ff .ttf`
if [ -s $f.oin ]; then
    list=`grep @list $f.def | cut -f2`
    if [ "$list" != "" ]; then
	larg="-l$list"
	grep ^$list sl/lists.tsv | cut -f1-2,9 | sort -u >$list.tsv
    fi
    echo $bin/ofpx $larg -d${ORACC}/lib/data/$f.def '<'$f.oin
    $bin/ofpx $larg -d${ORACC}/lib/data/$f.def -x$f.ofp <$f.oin
    rm -f *.{names,ucode,namuni,lig} *-ttf.txt *-ofp.in
    $bin/ofp2im.sh $f
fi
