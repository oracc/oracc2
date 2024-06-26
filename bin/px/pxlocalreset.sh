#!/bin/sh
project=$1
if [ "$project" = "" ]; then
    echo $0: must give project on command line. Stop.
    exit 1
fi
cache=$ORACC_P4_CACHE
if [ -d $cache ]; then
    echo Resetting P4 cache for $project ...
    sudo rm -fr ${cache}/$project/* ${cache}/htm/$project/*
    mkdir -p ${cache}/$project ${cache}/htm/$project
    sudo chown www-data:oracc ${cache}/$project ${cache}/htm/$project
    sudo chmod +s ${cache}/$project ${cache}/htm/$project
    sudo chmod g+w ${cache}/$project ${cache}/htm/$project
    ls -ld ${cache}/$project ${cache}/htm/$project
    exit 0
else
    echo $0: no local cache to reset. Stop.
    exit 1
fi
