#!/bin/sh
if [ -r 00cat/data.tsv ]; then
    cut -f1 00cat/data.tsv | grep ^[0-9] >00lib/outlined.lst
else
    xmllint -xpath '*/*/*[1]/text()' 00lib/data.xml >00lib/outlined.lst
fi
