#!/bin/sh
set 01bld/*/summaries.xml
if [ "$1" != "01bld/*/summaries.xml" ]; then
    for a in $* ; do
	d=`dirname $a`
	g=`basename $d`
	cbditem $a | dbix -d 02pub/cbd/$g -n etm -s
    done
fi
