#!/bin/sh
#
# Create a -pos page for each list
#
rocox -x fonts <$ORACC/lib/data/list-fonts.tsv >02xml/list-fonts.xml
while read l f
do
    if [ "$f" != "-" ]; then
	echo processing list $l using font $f
	grep "^$l[0-9]" 02pub/lists.tsv | sort -u \
	    | sort -t'	' -n -k3 >01tmp/$l.tsv
	rocox -h -x table -n \
	      -R'<tr><td>%1</td><td>%5</td><td class="ofs-noto ofs-150">%7</td><td class="ofs-'$f' ofs-200">%7</td><td>%6</td><td>%9</td></tr>' \
	      <01tmp/$l.tsv >02xml/$l.xml
    fi
done < $ORACC/lib/data/list-fonts.tsv 
xsltproc $ORACC/lib/scripts/sxw-list-pages.xsl 02xml/sl.xml
