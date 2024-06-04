#!/bin/sh
set 01bld/*/summaries.xml
if [ "$1" != "01bld/*/summaries.xml" ]; then
    for a in $* ; do
	d=`dirname $a`
	g=`basename $d`
	cbditem $a | tee 01tmp/$g.etm | dbix -d 02pub/cbd/$g -n etm -s
    done
fi
rm -f 01tmp/qpn-x-*.etm
rm -f 01tmp/oxid.tab
for a in 01tmp/*.etm ; do
    g=`basename $a .etm`
    cut -f1 $a | sed "s/$/	$g/" >>01tmp/oxid.tab
done
dbix -d 02pub/cbd -n oxid 01tmp/oxid.tab
