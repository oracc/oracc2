#!/bin/sh
source ~/afdko_env/bin/activate
b=`basename $1 .ttf`
if [ -r $b.ttx ]; then
    rm -f $b.ttx
fi
ttx -f -q $b.ttf
xsltproc ttx2oin.xsl $b.ttx >$b.oin
gzip -f -9 $b.ttx
