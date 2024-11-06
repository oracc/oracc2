#!/bin/sh
source ~/afdko_env/bin/activate
for a in `cat oracc-ttf.lst` ; do
    b=`basename $a .ttf`
    ttx -f -q $a
    xsltproc ttx2oin.xsl $b.ttx >$b.oin
    gzip -9 $b.ttx
done
