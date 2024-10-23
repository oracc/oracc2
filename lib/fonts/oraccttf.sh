#!/bin/sh
f=$1
showttfx $f.ttf >$f-ttf.txt
#grep '^ Glyph' ttf.txt | sed 's/^.*> //' | grep U+[1F]
#grep '<-' ttf.txt | cut -d'(' -f2 | cut -d')' -f1
#grep '\.[0-9]' ttf.txt | grep Base | cut -d'(' -f2 | cut -d')' -f1

grep ^Glyph $f-ttf.txt | grep U+[1F] | sed 's/^.\+U+/U+/' >$f-data.txt
grep ^Glyph $f-ttf.txt | grep _ | cut -d'"' -f2 >>$f-data.txt
grep ^Glyph $f-ttf.txt | grep -F . | cut -d'"' -f2 | grep '^u[^n]' >>$f-data.txt
