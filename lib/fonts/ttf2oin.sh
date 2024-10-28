#!/bin/sh
ff=$1
if [ "$ff" == "" ]; then
    echo $0: must give basename of .ttf file on command line. Stop.
    exit 1;
fi
f=`basename $ff .ttf`
showttfx $f.ttf >$f-ttf.txt
grep '^Glyph [0-9]\+ is named [0-9]\+ ' $f-ttf.txt | \
    sed 's/^.*\( [0-9]\+\) is named [0-9]\+ /\1	/' | sed 's/^ //' | sort -n >$f.names
if [ -s $f.names ]; then
    grep '^ Glyph [0-9]\+ ->' $f-ttf.txt | \
	sed 's/ Glyph\( [0-9]\+\) -> /\1	/' | sed 's/^ //' | sort -n >$f.ucode
    join -t'	' -j 1 $f.names $f.ucode | cut -f2,3 >$f.namuni
else
    grep '^Glyph [0-9]\+ (name' $f-ttf.txt | cut -d' ' -f 6,8 | tr -d '"'>$f.namuni
fi
grep Ligature $f-ttf.txt | grep '<-' | sed 's/^[ \t]\+Ligature glyph//' | sed 's/ [0-9]\+//g' | \
    sed 's/) (/_/g' | tr -d '()' | sed 's/^ //' >$f.lig
# process ligatures first because they may be needed for non-lig feature entries
sort $f.lig | cat - $f.namuni | sed 's/ \+$//' | sed 's/uni/u/g' >$f.oin
