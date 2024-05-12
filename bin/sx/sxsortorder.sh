#!/bin/dash
project=`oraccopt`
set 00lib/*.asl
if [ "$1" != "00lib/*.asl" ]; then
    sx -So $1 | sort -k2 -n -u | cut -f1,3 | tr '	' _ | rocox -x x |
	xsltproc -stringparam project $project ${ORACC_BUILDS}/lib/scripts/sxweb-sortorder.xsl - \
		 >signlist/00web/signlist-x-sortorder.xml
fi
# | pr -s'	' -t -2 -l 1000
