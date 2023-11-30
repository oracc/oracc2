#!/bin/dash
#
# mcu-lemm.sh checks the glossary files and updates the 02pub/*-lemm.sig
# lemmatization data.
#
projtype=`oraccopt . type`

if [ "$projtype" = "signlist" ]; then
    exit 0;
elif [ "$projtype" = "portal" ]; then
    exit 0;
fi

if [ "$projtype" = "superglo" ]; then
    true;
else
    set 00lib/*.glo
    if [ "$1" != "00lib/*.glo" ]; then
	echo $0: running o2-cbdpp.sh
	o2-cbdpp.sh
    else
	echo $0: skipping o2-cbdpp.sh because no 00lib/*.glo
    fi
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
