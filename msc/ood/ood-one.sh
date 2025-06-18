#!/bin/sh
project=`oraccopt`
if [ ! -r 00lib/data.xml ]; then
    echo $0: no 00lib/data.xml
    exit 0
fi
mkdir -p $ORACC/$project/01bld/lists
mkdir -p $ORACC/pub/$project/cat
cp 00lib/outlined.lst 01bld/lists
cp 00lib/data.xml 02pub
$ORACC/bin/xmdmanager2.plx
$ORACC/bin/qgdfx -p $project < 02pub/data.xml
sudo chmod -R o+r 02pub/cat
pxp3p4.sh $project
pxreset.sh $project
o2-web.sh
if [ ! -r 02www/lists/outlined.lst ]; then
    mkdir -p 02www/lists
    cp 00lib/outlined.lst 02www/lists
fi
