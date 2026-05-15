#!/bin/sh

echo $0 $*

if [ -r .ax ]; then
    oxax=ax
    g=-g
else
    oxax=oxx
fi

if [ "$1" == "-m" ]; then
    shift
    args=-cm
else
    args=-c
fi

case $1 in
    good|ok)
	cklist=ax.ok
	shift
    ;;
    bad|no)
	cklist=ax.no
	shift
    ;;
    *)
	echo $0: bad argument $1. Stop.
	exit 1
    ;;
esac

if [ -d "00atf" ]; then
    batch=`oraccopt . atf-batch`
    if [ "$batch" = "yes" ]; then
	for a in 01bld/atf-batch-*.lst ; do
	    $oxax $args $g -I$a
	done
    else
	files=`find 00atf -maxdepth 1 -type f | grep '.\(ods\|.atf\)$' | grep -v ods.atf`
	$oxax $args $files
    fi
fi
