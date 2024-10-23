#!/bin/sh
f=$1
showttfx $f.ttf >$f-ttf.txt
#grep '^ Glyph' ttf.txt | sed 's/^.*> //' | grep U+[1F]
#grep '<-' ttf.txt | cut -d'(' -f2 | cut -d')' -f1
#grep '\.[0-9]' ttf.txt | grep Base | cut -d'(' -f2 | cut -d')' -f1

grep ^Glyph $f-ttf.txt | grep U+[1F] | sed 's/^.\+U+/U+/' >$f-data.txt
if [ ! -s $f-data.txt ]; then
    grep '^ Glyph' $f-ttf.txt | grep -- '->' | grep U+[1F] | sed 's/^.\+U+/U+/' >$f-data.txt
fi
grep ^Glyph $f-ttf.txt | grep _ | cut -d'"' -f2 >>$f-data.txt
grep ^Glyph $f-ttf.txt | grep -F . | cut -d'"' -f2 | grep '^u[^n]' >>$f-data.txt
grep Ligature $f-ttf.txt | grep '<-' | sed 's/^.\+<-//' | sed 's/ [0-9]\+//g' | \
    sed 's/) (/_/g' | tr -d '()' | sed 's/^ //' >>$f-data.txt
grep -F .ss $f-ttf.txt | grep -- '->' | cut -d'>' -f2 | \
    cut -d'(' -f2 | tr -d ')' | grep ^u | sort -u >>$f-data.txt
