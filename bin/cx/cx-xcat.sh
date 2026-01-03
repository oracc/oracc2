#!/bin/sh
#
# Create a minimal catalogue of X-IDs in 01bld/cat/auto-x.tsv
#
autox=01bld/cat/auto-x.tsv
rm -f $autox
set 00atf/*.atf
if [ "$1" != "00atf/*.atf" ]; then
    cat 00atf/*.atf | atfdatax -x >$autox
fi
