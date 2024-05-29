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
set 00atf/*.atf
if [ "$1" != "00atf/*.atf" ]; then
    cat 00atf/*.atf | atfdatax >02pub/atf-data.tab
    cut -f1,3 <02pub/atf-data.tab | dbix -d 02pub -n trs -s
fi
libscripts=${ORACC_BUILDS}/lib/scripts
#xsltproc $libscripts/p3-trans-p4.xsl 02xml/trans-select.xml \
#	 >02xml/p4-trans-select.xml
#chmod o+r 02xml/p4-trans-select.xml
# This is called by p3-project-data.plx
# pxlangs.sh
#pxsortercodes.sh $project
# p3-project-data.plx doesn't do the following bit
for a in 02pub/cbd/* ; do
    pxentrycounts.sh $a
    pxentryindexes.sh $a
done
