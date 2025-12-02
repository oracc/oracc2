#!/bin/sh
t=$1
if [ "$t" = "" ]; then
    echo $0: must give .tsv file on command line. Stop.
    exit 1
fi
rocox -fov -x o:records -r o:record $1 <$t |
    xsltproc ${ORACC}/lib/scripts/ood-xml-id.xsl - >data.xml
