#!/bin/sh

echo $0 $*

if [ "$1" == "-m" ]; then
    args=-cm
else
    args=-c
fi

if [ -d "00atf" ]; then
    batch=`oraccopt . atf-batch`
    if [ "$batch" = "yes" ]; then
	for a in 01bld/atf-batch-*.lst ; do
	    oxx $args -I$a
	done
    else
	files=`find 00atf -maxdepth 1 -type f | grep '.\(ods\|.atf\)$' | grep -v ods.atf`
	oxx $args $files
    fi
fi

