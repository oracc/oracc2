#!/bin/sh
set -x
if [ "$AFDKO_PATH" = "" ]; then
    AFDKO_PATH=~
fi
ff=$1
if [ "$ff" == "" ]; then
    echo $0: must give .ttf file on command line. Stop.
    exit 1;
fi
x=`basename $ff .ttf`.ttx
source $AFDKO_PATH/afdko/bin/activate
ttx -o $x $ff
f=`basename $ff .ttf`
xsltproc $ORACC/lib/scripts/ttx2oin.xsl $x >$f.oin
