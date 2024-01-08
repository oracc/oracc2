#!/bin/sh
gtoks=$1
if [ "$1" = "" ]; then
    echo $0: must give output of toks-g on command line. Stop.
    exit 1
fi
project=`head -1 $gtoks | cut -f2`
exec >corpus.asl
echo "@project $project"
echo "@signlist corpus"
echo
grep ^o $gtoks | grep '%sl:' | cut -f2 | cut -d: -f2 | sig2asl
