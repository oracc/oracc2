#!/bin/sh
set 00etc/*-pos.xml
if [ "$1" != "00etc/*-pos.xml" ]; then
    for a in 00etc/*-pos.xml ; do
	b=`basename $a .xml`
	c=`basename $b -pos`
	if [ -x 00bin/$b.sh ]; then
	    00bin/$b.sh
	elif [ -x ${ORACC}/bin/$b.sh ]; then
	    $b.sh
	fi
	xmllint --encode UTF-8 --xinclude --noxincludenode $a | \
	    xsltproc -stringparam project `oraccopt` \
		     ${ORACC}/lib/scripts/sxweb-page-o-signs.xsl - >00lib/signlist-x-$c.xml
    done
fi
