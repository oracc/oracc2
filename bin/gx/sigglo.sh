#!/bin/bash
input=$1
if [ "$input" = "" ]; then
    echo $0: must give input file on command line. Stop.
    exit 1
fi
ext=${input##*.}
if [ "$ext" = "sig" ] || [ "$ext" = "tix" ]; then
    echo $0: input $input matched '.sig|tix'
    for a in `grep '^[@{]' $input | cut -f1 | siglangx -`; do
	mkdir -p 01bld/$a
    done
    grep '^[@{]' $input | cut -f1 | sigglox -p gudea
fi
