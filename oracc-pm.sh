#!/bin/sh
ORACC=$1
srcdir=$2

shopt -s nullglob

function install_one_pm {
    echo Oracc II has Perl $1; please refactor/remove.
}

pm=`ls *.pm`
if [ "$pm" != "" ]; then
    for each in *.pm ; do install_one_pm $each ; done
fi
