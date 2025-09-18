#!/bin/sh
if [ "$1" = "" ]; then
    echo $0: must give .ofp file on command line. Stop
    exit 1
fi
xsltproc $ORACC/lib/scripts/ofp2uni.xsl $1 >`basename $1 .ofp`.uni
