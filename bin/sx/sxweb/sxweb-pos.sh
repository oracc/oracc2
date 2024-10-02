#!/bin/sh
for a in 00etc/*.pos ; do
    b=`basename $a .pos`
    if [ -x 00bin/$b.sh ]; then
	00bin/$b.sh
    elif [ -x ${ORACC}/bin/$b.sh ]; then
	$b.sh
    fi
    xmllint --encode UTF-8 --xinclude --noxincludenode $a | \
	xsltproc ${ORACC}/lib/scripts/sxweb-page-o-signs.xsl - >00lib/$b.xml
done
