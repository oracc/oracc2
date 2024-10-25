#!/bin/sh
ff=$1
if [ "$ff" == "" ]; then
    echo $0: must give basename of .ttf file on command line. Stop.
    exit 1;
fi
f=`basename $ff .ttf`
ttf2ofp-in.sh $f
if [ -s $f-ofp.in ]; then
    ofpx -t <$f-ofp.in >$f.ofp
    ofpx -x <$f-ofp.in >$f.ofpx
    rm -f *.{names,ucode,namuni,lig} *-ttf.txt *-ofp.in
fi
