#!/bin/sh
p=$1
l=$2
if [ "$p" == "" ] || [ "$l" == "" ]; then
    echo $0: must give project and language on command line. Stop.
    exit 1
fi
s=${ORACC}/$p/01bld/$l/summaries.xml 
if [ ! -r $s ]; then
    echo $0: no such file $s. Stop.
    exit 1
fi
crpsigns.sh $p
needuni.sh | grep -f - need
