#!/bin/sh
#
# List OIDs that occur in the font argument excluding atoms and
# splitting seq members so the list is actually the atoms needed by
# sequences.
#
# The argument should be a .ofp file
#
f=$1
if [[ "$f" == "" ]]; then
    echo "$0: must give .ofp file on command line. Stop."
    exit 1
fi
xmllint -xpath "*/*[contains(@xml:id,'.')]/@oid" $f | sed 's/^.*"\(.*\)"$/\1/' | sort -u >f.seq
grep -f f.seq ${ORACC}/osl/02pub/unicode.tsv | cut -f 1 | tr '.' '\n' | sort -u | sed 's/^x/U+/' >f.sequ
grep -f f.sequ ${ORACC}/osl/02pub/unicode.tsv | cut -f 3 | sort -u 
