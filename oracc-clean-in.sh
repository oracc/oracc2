#!/bin/sh
shopt -s nullglob
in=*.in
if [ "$in" != "" ]; then
    for a in *.*.in ; do rm -f `basename $a .in` ; done
fi
rm -f *.tab.[ch] *.lex.[ch] *.output
if [ -x cleanme.sh ]; then
    ./cleanme.sh
fi
