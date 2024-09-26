#!/bin/sh
project=$1
asl=$2
if [ "$project" = "" ]; then
    project=osl
fi
if [ "$asl" = "" ]; then
    asl=02xml/sl.xml
fi
libscripts=$ORACC_BUILDS/lib/scripts
xsltproc -stringparam project $project -stringparam projesp "$ORACC_BUILDS/$project" $libscripts/sxweb-signs.xsl $asl
