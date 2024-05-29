#!/bin/dash
cat 00atf/*.atf | atfdatax >02pub/atf-data.tab
cut -f1 02pub/atf-data.tab | \
    lx -q -p `oraccopt` >01bld/lists/have-atf.lst
grep '#lem' 02pub/atf-data.tab | cut -f1 | \
    lx -q -p `oraccopt` >01bld/lists/have-lem.lst
if [ -r 00lib/umbrella.lst ]; then
    lx-umbrella.sh 00lib/umbrella.lst `oraccopt`
elif [ -r 00lib/search.lst ]; then
    lx-umbrella.sh 00lib/search.lst `oraccopt`
elif [ -r 00lib/proxy.lst ]; then
    grep -v @ 00lib/proxy.lst >01bld/lists/proxy-atf.lst
    grep @ 00lib/proxy.lst >00lib/proxy-cat.lst
fi
if [ -r 01bld/proxy.lst ]; then
    grep -v @ 01bld/lists/proxy.lst >01bld/lists/proxy-atf.lst
    grep @ 01bld/lists/proxy.lst >00lib/proxy-cat.lst
fi
