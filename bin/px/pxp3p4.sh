#!/bin/dash
#
# Translate P3 project-data to P4 attributes/IDs.
#
# This is a temporary measure until p3-project-data.plx is rewritten.
project=$1
if [ "$project" = "" ]; then
    echo $0: must give project argument. Stop.
    exit 1;
fi
libscripts=${ORACC_BUILDS}/lib/scripts
xsltproc $libscripts/p3-trans-p4.xsl 02xml/trans-select.xml \
	 >02xml/p4-trans-select.xml
chmod o+r 02xml/p4-trans-select.xml
pxsortercodes.sh $project
