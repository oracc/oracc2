#!/bin/sh
dir=01bld/www/cbd/sux
htm=articles.html
art=$dir/$htm
if [ -s $art ]; then
    lpxi=${ORACC_BUILDS}/lib/scripts/lex-provides-xi.xsl
    if [ -r $lpxi ]; then
	echo $0 running "cd $dir ; xsltproc -o $htm $lpxi $htm"
	(cd $dir ; xsltproc -o $htm $lpxi $htm)
    else
	echo $0 no $lxpi. Stop.
    fi
else
    #    echo $0 no $art. Stop.
    exit 0
fi
