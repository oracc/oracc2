#!/bin/sh
#
# List OIDs that occur in the font argument excluding sequences.
#
# The argument should be a .ofp file
#
f=$1
if [[ "$f" == "" ]]; then
    echo "$0: must give .ofp file on command line. Stop."
    exit 1
fi
xmllint -xpath "*/*[not(contains(@xml:id,'.'))]/@oid" $f | sed 's/^.*"\(.*\)"$/\1/' | sort -u
