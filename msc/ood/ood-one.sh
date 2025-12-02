#!/bin/sh
project=`oraccopt`
if [ "$project" = "" ]; then
    echo $0: not in a project directory. Stop.
    exit 1
fi
type=`oraccopt . type`
if [ "$type" != "ood" ]; then
    echo $0: $project is not an ood project. Stop.
    exit 1
fi
mkdir -p $ORACC/$project/01bld/lists
mkdir -p $ORACC/pub/$project/cat
cp 00lib/outlined.lst 01bld/lists
if [ -d 00lib/data.d ] ; then
    sudo chmod -R o+r 00lib/data.d
    rm -f 02pub/data.d
    ln -sf `pwd`/00lib/data.d 02pub
fi
if [ -r 00lib/data.xml ]; then
    cp 00lib/data.xml 02pub
    sudo chmod o+r 02pub/data.xml
fi
$ORACC/bin/xmdmanager2.plx
$ORACC/bin/qgdfx -p $project < 02pub/data.xml
sudo chmod -R o+r 02pub/cat
o2-web.sh
if [ ! -r 02www/lists/outlined.lst ]; then
    mkdir -p 02www/lists
    cp 00lib/outlined.lst 02www/lists
fi
