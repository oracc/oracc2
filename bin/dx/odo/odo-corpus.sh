#!/bin/dash
echo $0 $*
o2-corpus.sh $*
t=`oraccopt . atf-tok`
if [ "$t" = "yes" ]; then
    cat 01bld/lists/xtfindex.lst | tok-g >01tmp/graphemes.tok
fi
sxweb.sh
