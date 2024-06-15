#!/bin/dash
#
# Generate P4 project data in addition to what p3-project-data does
#
echo $0 $*
project=$1
if [ "$project" = "" ]; then
    echo $0: must give project argument. Stop.
    exit 1;
fi
libscripts=${ORACC_BUILDS}/lib/scripts
# p3-project-data.plx doesn't do the following bit
cbd-items.sh
for a in 02pub/cbd/* ; do
    if [ -r $a/entry_ids.lst ]; then
	pxentrycounts.sh $a
	pxentryindexes.sh $a
    fi
done
cut -f1,3 02pub/atf-data.tab | sed 's/@[/a-z0-9]\+//' | grep '	' | sed 's/^.*://' | \
    tee 02pub/trs.tsv | dbix -s -d 02pub -n trs
cut -d@ -f1 <02pub/atf-data.tab | tr : '	' | rocox -C21 | \
    tee 02pub/prx.tsv | dbix -s -d 02pub -n prx
rm -fr 02pub/p4.d
pxprecompute.sh
chmod -R o+r 02pub
