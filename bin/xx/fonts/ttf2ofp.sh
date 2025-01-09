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
	grep ^$list sl/lists.tsv | cut -f1-2,9 >$list.tsv
    fi
    echo $bin/ofpx $larg -t$f.ofp -x$f.ofpx '<'$f.oin
    $bin/ofpx $larg -t$f.ofp -x$f.ofpx <$f.oin
    rm -f *.{names,ucode,namuni,lig} *-ttf.txt *-ofp.in
    $bin/ofp2im.sh $f
fi
