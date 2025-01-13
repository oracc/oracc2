#!/bin/sh
imbase=`grep @css $1.def | cut -f2`
xsltproc $ORACC/lib/scripts/ofpx2im.xsl $1.ofpx | \
    cat $1.def - >im-$imbase.tab
