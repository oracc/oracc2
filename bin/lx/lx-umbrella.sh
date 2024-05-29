#!/bin/dash
l=$1
p=$2
if [ "$p" != "" ]; then
    p="$p/"
fi
rm -f 01bld/lists/proxy.lst
exec >>01bld/lists/proxy.lst
for a in `tr ' ' '\n' <00lib/$1.lst` ; do
    lx-atf.sh $p$a
done
if [ -r 00lib/proxy.lst ]; then
    cat 00lib/proxy.lst
fi
