#!/bin/sh
for a in *.pos ; do
    b=`basename $a .pos`
    if [ -x 00bin/$a.sh ]; then
	00bin/$a.sh
    elif [ -x ${ORACC}/bin/$a.sh ]; then
	$a.sh
    fi
    xsltproc sxweb-page-o-signs.xsl $a >00lib/$b.xml
done
