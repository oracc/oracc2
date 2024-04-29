#!/bin/sh
project=$1
if [ "$project" = "" ]; then
    echo $0: must give project on command line. Stop.
    exit 1
fi

echo Resetting $project ...
sudo rm -fr ${ORACC_BUILDS}/www/is.d/$project
