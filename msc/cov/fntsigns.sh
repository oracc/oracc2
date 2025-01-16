#!/bin/sh
#
# List OIDs that occur in the font argument.
#
# The argument should be a .ofp file
#
f=$1
if [[ "$f" == "" ]]; then
    p=`oraccopt`
    if [[ "$f" == "" ]]; then
	echo "$0: either give project on command-line or run me in a project directory. Stop."
	exit 1
    fi
fi
xmllint -xpath '*/*/@oid' $f | sed 's/^.*"\(.*\)"$/\1/' | sort -u
