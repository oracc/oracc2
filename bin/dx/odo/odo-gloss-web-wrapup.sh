#!/bin/dash

echo $0 $*

dwww=${ORACC_BUILDS}/$p/02www/cbd

if [ -d 01tmp/lex/cbd ]; then
    for l in 01tmp/lex/cbd/* ; do
	lang=`basename $l`
	echo l=$l
	for t in $l/* ; do
	    echo type=$t
	    mv -v $t $dwww/$lang
	done
    done
    cp 01tmp/lex/*provides*.xml 02www
fi

if [ -r 00etc/word-pages.lst ]; then
    esp2-word-pages.plx
fi

oid-index-cbd.sh `oraccopt`
