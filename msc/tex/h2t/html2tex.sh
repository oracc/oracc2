#!/bin/sh
t=$1
if [ -r $1.xml ]; then
    xsltproc ${ORACC}/lib/scripts/html2tex.xsl $1.xml >$1.tex
else
    echo $0: must give basename of XML file. Stop.
    exit 1
fi
