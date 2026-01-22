#!/bin/sh
#
# Create a minimal catalogue of X-IDs in 01bld/cat/auto-x.tsv
#
autox=01tmp/00cat/auto-x.tsv
rm -f $autox
set 00atf/*.atf
if [ "$1" != "00atf/*.atf" ]; then
    havex=01tmp/00cat/have-x
    if [ -s 01tmp/00cat/local-x.tsv ]; then
	grep ^X 01tmp/00cat/local-x.tsv | cut -f1 >$havex
    else
	cat </dev/null >$havex
    fi
    cat 00atf/*.atf | atfdatax -x -p `oraccopt` | grep -v -f $havex >$autox
fi
