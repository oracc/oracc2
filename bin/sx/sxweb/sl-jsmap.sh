#!/bin/sh
xsltproc $ORACC_BUILDS/lib/scripts/sl-jsmap-data.xsl $1 | sed 's/"/\\"/' | sed "s/'/\\'/" | jsmapx
