#!/bin/sh
#
# Create a -pos page for each list
#
rocox -x fonts <$ORACC/lib/data/list-fonts.tsv >02xml/list-fonts.xml
while read l f
do
    if [ "$f" != "-" ]; then
	echo processing list $l using font $f
	ltsv=01tmp/$l.tsv
	head -1 02pub/lists.tsv >$ltsv
	grep '^\(►\)\?'$l'[0-9]' 02pub/lists.tsv | sort -u \
	    | sort -t'	' -n -k3 >>$ltsv
	rocox -f -x list -r entry <01tmp/$l.tsv >01tmp/$l.xml
	xsltproc -stringparam ofpx $ORACC/lib/data/*$l*.ofp \
		 -stringparam font ofs-$f \
		 $ORACC/lib/scripts/sxw-list-table.xsl 01tmp/$l.xml \
		 >02xml/$l.xml
    fi
done < $ORACC/lib/data/list-fonts.tsv 
xsltproc $ORACC/lib/scripts/sxw-list-pages.xsl 02xml/sl.xml
