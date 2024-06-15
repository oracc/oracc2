#!/bin/sh
for l in `locate outlined.lst | grep home | grep -v '^\(/home/oracc\|/home/stinney\)' | grep 00lib/outlined.lst` ; do
    d=`dirname $l`
    (cd $d
     if [ -r outlined.lst ] && [ ! -s outlined.lst ]; then
	 rm -vf $l
     fi
    )
done
