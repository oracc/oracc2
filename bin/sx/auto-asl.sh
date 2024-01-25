#!/bin/sh
gtoks=$1
if [ "$gtoks" = "" ]; then
    echo $0: must give output of tokx on command line. Stop.
    exit 1
fi
project=`head -1 $gtoks | cut -f2`
if [ "$project" = "" ]; then
    project=`oraccopt`
fi
tok2asl -P $project -S auto $gtoks >01tmp/auto.asl
sx -e -c 01tmp/auto.asl >>01tmp/auto.asl
