#!/bin/sh
project=$1
if [ "$project" = "" ]; then
    echo $0: must give project on command line. Stop.
    exit 1
fi

echo Resetting P4 cache for $project ...
sudo rm -fr ${ORACC_BUILDS}/www/{htm,is.d}/$project/*
mkdir -p ${ORACC_BUILDS}/www/{htm,is.d}/$project/*
sudo chown www-data:oracc ${ORACC_BUILDS}/www/{htm,is.d}/$project
sudo chmod +s ${ORACC_BUILDS}/www/{htm,is.d}/$project
sudo chmod g+w ${ORACC_BUILDS}/www/{htm,is.d}/$project
ls -ld ${ORACC_BUILDS}/www/{htm,is.d}/$project
