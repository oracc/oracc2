#!/bin/dash
b=${ORACC_BUILDS}/$1/01bld/lists
>&2 echo $0: working in $b
if [ -r $b/have-atf.lst ]; then
    cat $b/have-atf.lst
fi
if [ -r $b/proxy-atf.lst ]; then
    cat $b/proxy-atf.lst
fi
