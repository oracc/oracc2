#!/bin/sh
label=`grep @label $1.def | cut -f2`
imbase=`grep @css $1.def | cut -f2`
xsltproc -stringparam label $label $ORACC/lib/scripts/ofp2im.xsl $1.ofp | \
    cat $1.def - >im-$imbase.tab
