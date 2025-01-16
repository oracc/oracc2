#!/bin/sh
#
# Produce a list of OIDs of missing atoms a font needs to render its sequences
#
f=$1
if [[ "$f" == "" ]]; then
    echo "$0: must give .ofp file on command line. Stop."
    exit 1
fi
fntatom.sh $f >f.atom
fntseqs.sh $f >f.seqo
comm -23 f.seqo f.atom
