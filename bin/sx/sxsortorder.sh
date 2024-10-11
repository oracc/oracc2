#!/bin/dash
project=`oraccopt`
set 00lib/*.asl
if [ "$1" != "00lib/*.asl" ]; then
    asl=$1
else
    if [ -r 01tmp/csl.asl ]; then
	asl=01tmp/csl.asl
    fi
fi
if [ "$asl" != "" ]; then
    sx -So $asl | sort -k2 -n -u | cut -f1,3 | tr '	' _ | rocox -x x |
	xsltproc -stringparam project $project ${ORACC_BUILDS}/lib/scripts/sxweb-sortorder.xsl - \
		 >signlist/00web/signlist-x-sortorder.xml
fi
