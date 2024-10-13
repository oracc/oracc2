#!/bin/sh
p=$1
if [ "$p" == "" ]; then
    echo $0: must give project on command line. Stop.
    exit 1
fi
crpsigns.sh $p
needuni.sh | grep -f - need
