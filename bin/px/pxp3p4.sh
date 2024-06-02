#!/bin/dash
#
# Generate P4 project data in addition to what p3-project-data does
#
project=$1
if [ "$project" = "" ]; then
    echo $0: must give project argument. Stop.
    exit 1;
fi
#set 00atf/*.atf
#if [ "$1" != "00atf/*.atf" ]; then
#    cat 00atf/*.atf | atfdatax >02pub/atf-data.tab
#    cut -f1,3 <02pub/atf-data.tab | dbix -d 02pub -n trs -s
#fi
libscripts=${ORACC_BUILDS}/lib/scripts
# p3-project-data.plx doesn't do the following bit
cbd-items.sh
for a in 02pub/cbd/* ; do
    if [ -r $a/entry_ids.lst ]; then
	pxentrycounts.sh $a
	pxentryindexes.sh $a
    fi
done
