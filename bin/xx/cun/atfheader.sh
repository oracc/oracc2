#!/bin/sh
atf=$1
if [[ "$atf" == "" ]]; then
    echo $0: must give ATF file. Stop.
    exit 1
fi
