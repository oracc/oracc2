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
merge=`oraccopt . asl-merge`
if [ "$merge" != "" ]; then
    mopt="-m $merge"
fi

if [ -r "00lib/auto-asl.txt" ]; then
    h="-h 00lib/auto-asl.txt"
fi

tok2asl $h -P $project $mopt -S auto $gtoks >01tmp/auto.asl
sx -p osl -e -c 01tmp/auto.asl >>01tmp/auto.asl
libdata=${ORACC_BUILDS}/lib/data
for a in signlist-structure-top.xml signlist-structure-bot.xml ; do
    if [ ! -r 00lib/$a ]; then
	cp $libdata/$a 00lib
    fi
done
