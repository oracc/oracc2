#!/bin/sh
#
# Query cdli.earth for numbers that aren't found in the catalogue to
# find relocations. The output format is
#
# [ORIGINAL_ID]	[RELOCATED_ID1] ([RELOCATED_ID2]...)
#
# Multiple relocations indicate that the current ID is the last
# relocation in the row.
#
if [ "$1" = "" ]; then
    echo $0: must give list of P-numbers to check for relocation. Stop.
    exit 1
fi
d=`isogmt`
for a in `cat $1`; do
    >&2 echo $0: looking up https://cdli.earth/$a
    curl -sI -L https://cdli.earth/$a | grep ^location: | cut -d/ -f5 | \
	tr -d '\r' | tr '\n' '\t' | sed 's/	$/\n/' >dups-$d.tsv
done
