#!/bin/sh
csldist -t 00lib/pcsl-template.txt <02pub/csl.tix >01tmp/csldist.xml
xsltproc $ORACC/lib/scripts/pcsl-csldata.xsl 01tmp/csldist.xml >02xml/csldist.xml
