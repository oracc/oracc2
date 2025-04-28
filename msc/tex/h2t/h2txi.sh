#!/bin/sh
outf=h2t.html
printf '<h2t-wrapper xml:base="/Users/stinney/orc/www/pcsl/" xmlns:xh="http://www.w3.org/1999/xhtml" xmlns:xi="http://www.w3.org/2001/XInclude">' >$outf
grep ^+ h2t-esp.url | cut -f2 | cat - h2t-add.url | sed 's#^/##' \
    | sed 's#^#<xi:include href="#' | sed 's#$#.xml"/>#' | tr -d '\n' >>$outf
printf '</h2t-wrapper>' >>$outf
