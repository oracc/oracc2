#!/bin/sh
ff=$1
if [ "$ff" == "" ]; then
    echo $0: must give basename of .ttf file on command line. Stop.
    exit 1;
fi
f=`basename $ff .ttf`
ttf2oin.sh $f
if [ -s $f.oin ]; then
    list=`grep @list $f.def | cut -f2`
    if [ "$list" != "" ]; then
	larg="-l$list"
	grep ^$list /home/oracc/osl/02pub/sx-lists.tsv | cut -f1-2 >$list.tsv
    fi
    ofpx $larg -t$f.ofp -x$f.ofpx <$f.oin
    rm -f *.{names,ucode,namuni,lig} *-ttf.txt *-ofp.in
    ./ofp2im.sh $f
fi
