#!/bin/sh
ff=$1
if [ "$ff" == "" ]; then
    echo $0: must give basename of .ttf file on command line. Stop.
    exit 1;
fi
f=`basename $ff .ttf`
ttf2oin.sh $f
if [ -s $f.oin ]; then
    ofpx -t$f.ofp -x$f.ofpx <$f.oin
    rm -f *.{names,ucode,namuni,lig} *-ttf.txt *-ofp.in
fi
