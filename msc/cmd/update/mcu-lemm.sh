#!/bin/sh

projtype=`oraccopt . type`

if [ "$projtype" == "superglo" ]; then
    true;
else
    echo $0: running o2-cbdpp.sh
    o2-cbdpp.sh
fi

echo $0: running l2p1-project-sig.sh
l2p1-project-sig.plx

echo $0: running l2p1-simple-from-project.sh
l2p1-simple-from-project.sh

if [ -r .nolemmdata ]; then
    rm -f .nolemmdata
else
    echo $0: running l2p1-lemm-data.plx -g -f
    l2p1-lemm-data.plx -g -f
fi
