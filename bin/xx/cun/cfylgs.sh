#!/bin/sh
# $1 = .xtf file; output to stdout
xsltproc $ORACC/lib/scripts/cfy-lgs.xsl $1
