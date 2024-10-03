#!/bin/sh
for a in 00etc/*-pos.xml ; do
    b=`basename $a .xml`
    c=`basename $b -pos`
    if [ -x 00bin/$b.sh ]; then
	00bin/$b.sh
    elif [ -x ${ORACC}/bin/$b.sh ]; then
	$b.sh
    fi
    xmllint --encode UTF-8 --xinclude --noxincludenode $a | \
	xsltproc ${ORACC}/lib/scripts/sxweb-page-o-signs.xsl - >00lib/signlist-x-$c.xml
done
