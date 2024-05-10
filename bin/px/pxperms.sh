#!/bin/sh
project=$1
if [ "$project" = "" ]; then
    echo $0: must give project on command line. Stop.
    exit 1
fi

echo Resetting P4 cache permissions for $project ...
sudo chown -R www-data:oracc ${ORACC_BUILDS}/www/{htm,is.d}/$project
sudo chmod -R +s ${ORACC_BUILDS}/www/{htm,is.d}/$project
sudo chmod -R g+rwx ${ORACC_BUILDS}/www/{htm,is.d}/$project
ls -ld ${ORACC_BUILDS}/www/{htm,is.d}/$project
