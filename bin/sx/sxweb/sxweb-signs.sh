#!/bin/sh
project=$1
asl=$2
libscripts=$ORACC_BUILDS/lib/scripts
xsltproc -stringparam project $project -stringparam projesp "$ORACC_BUILDS/$project" $libscripts/sxweb-signs.xsl $asl
