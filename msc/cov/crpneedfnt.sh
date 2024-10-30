#!/bin/sh
p=$1
if [ "$p" == "" ]; then
    echo $0: must give project on command line. Stop.
    exit 1
fi
f=$2
if [ ! -r $f ]; then 
    echo $0: must give full path to .ofpx file on command line. Stop.
    exit 1
fi
crpsigns.sh $p
needfnt.sh $f | grep -f - need
