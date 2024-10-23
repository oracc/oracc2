#!/bin/sh
ff=$1
if [ "$ff" == "" ]; then
    echo $0: must give basename of .ttf file on command line. Stop.
    exit 1;
fi
f=`basename $ff .ttf`
showttfx $f.ttf >$f-ttf.txt
grep ^Glyph $f-ttf.txt | grep U+[1F] | sed 's/^.\+U+/U+/' >$f-data.txt
if [ ! -s $f-data.txt ]; then
    grep '^ Glyph' $f-ttf.txt | grep -- '->' | grep U+[1F] | sed 's/^.\+U+/U+/' >$f-data.txt
fi
grep ^Glyph $f-ttf.txt | grep -F . | cut -d'"' -f2 | grep '^u[^n]' >$f-data.dot
grep Ligature $f-ttf.txt | grep '<-' | sed 's/^[ \t]\+Ligature glyph//' | sed 's/ [0-9]\+//g' | \
    sed 's/) (/_/g' | tr -d '()' | sed 's/^ //' >>$f-data.txt
grep '\.[0-9]\|\.ss\|\.cv' $f-ttf.txt | grep -- '->' | cut -d'>' -f2 | \
    cut -d'(' -f2 | tr -d ')' | grep -v '|\|"' | sort -u >>$f-data.dot
sort -u $f-data.dot | grep -v '^NULL\|uni0' >>$f-data.txt
rm -f $f-data.dot
