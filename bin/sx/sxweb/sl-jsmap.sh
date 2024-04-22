#!/bin/sh
xsltproc $ORACC_BUILDS/lib/scripts/sl-jsmap-data.xsl $1 | sed 's/"/\\"/' | sed "s/'/\\'/" | \
    sed 's/ś/ss/' | grep '^[a-z0-9]' | jsmapx m
