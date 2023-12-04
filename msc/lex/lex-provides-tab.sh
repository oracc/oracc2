#!/bin/dash
set $ORACC_BUILDS/www/dcclt/lex-provides-*.xml
rm -f provides.tab
for a in $* ; do
    type=`/bin/echo -n $a | sed 's#^.*provides-.*-\(.\).*$#\1#'`
    xmllint --format $a | grep 'id="o' | sed "s#^.*=\"\\(.*\\)\"/>#\1	$type#" >>provides.tab
done

