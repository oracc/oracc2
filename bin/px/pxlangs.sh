#!/bin/dash
#
# There should be a config option to give the langs explicitly in a defined order.
#
deflang=`oraccopt . trans-default-lang`
if [ "$deflang" = "" ]; then
    deflang=en
fi
pxlangfile=02pub/langs.txt
echo $deflang >$pxlangfile
cut -f2 01bld/lists/have-xtr.tab | sort -u | grep -v $deflang >>$pxlangfile
echo 'none' >>$pxlangfile
chmod o+r $pxlangfile
