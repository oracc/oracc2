#!/bin/sh
if [ -d 01bld/www/cbd/sux/articles.html ]; then
    lpxi=${ORACC_BUILDS}/lib/scripts/lex-provides-xi.xsl
    if [ -r $lpxi ]; then
	echo $0 running xsltproc $lpxi articles.html
	(cd 01bld/www/cbd/sux ; 
	 xsltproc ${ORACC_BUILDS}/lib/scripts/lex-provides-xi.xsl articles.html >xarticles.html ;
	 mv xarticles.html articles.html)
    else
	echo $0 no $lxpi. Stop.
    fi
fi
