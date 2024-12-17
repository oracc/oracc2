#!/bin/sh
bin=`dirname $0`
$bin/oslfiles.sh 
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
	grep ^$list /home/oracc/osl/02pub/lists.tsv | cut -f1-2 >$list.tsv
    fi
    $bin/ofpx $larg -t$f.ofp -x$f.ofpx <$f.oin
    rm -f *.{names,ucode,namuni,lig} *-ttf.txt *-ofp.in
    $bin/ofp2im.sh $f
fi
