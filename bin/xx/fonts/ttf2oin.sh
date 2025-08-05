#!/bin/sh
set -x
if [ "$AFDKO_PATH" = "" ]; then
    AFDKO_PATH=~
fi
ff=$1
if [ "$ff" == "" ]; then
    echo $0: must .ttf basename on command line. Stop.
    exit 1;
fi
f=`basename $ff .ttf`.ttf
x=`basename $ff .ttf`.ttx
source $AFDKO_PATH/afdko/bin/activate
ttx -o $x $f
o=`basename $ff .ttf`.oin
xsltproc $ORACC/lib/scripts/ttx2oin.xsl $x >$o
