#!/bin/sh
if [ -r .odolog ]; then
    log=`cat .odolog`
    if [ -r $log ]; then
	less $log
    else
	echo $0: log $log not found. Stop.
    fi
else
    echo $0: .odolog not found. Stop.
fi
