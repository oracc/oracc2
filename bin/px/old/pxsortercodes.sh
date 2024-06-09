#!/bin/dash
project=$1
if [ "$project" = "" ]; then
    echo $0: must give project argument. Stop.
    exit 1;
fi
oraccproj=${ORACC_BUILDS}/$project
libscripts=${ORACC_BUILDS}/lib/scripts
mkdir -p 01tmp
pubdir=$oraccproj/02pub
xmldir=$oraccproj/02xml
sorter=$xmldir/outline-sorter.xml
for a in p3OSdefault p3OSspecial ; do
    xmllint --xpath "//*[@id='$a']" $sorter >01tmp/$a.xml
    xmllint --xpath "/*/@value" 01tmp/$a.xml | cut -d'"' -f2 | tr ',' '\n' | cat -n \
	| tr -s ' ' | rocox -n -R 's/,%2,/%1/' | tr -d ' ' >01tmp/$a.sed
    sed 's/^.*value=//' 01tmp/$a.xml | cut -d'>' -f1 | cut -d' ' -f1 | tr '"', ',' \
	| sed 's/,/,,/g' | sort -u | grep ^, >01tmp/$a.val
    sed -f 01tmp/$a.sed 01tmp/$a.val | tr -d , | paste 01tmp/$a.val - \
	| sed 's/,,/,/g' | sed 's/^,//' | sed 's/,	/	/' >01tmp/$a.tab
    rocox -n -R 's/%1/%2/' <01tmp/$a.tab >01tmp/$a-sort.sed
    p4fn=`/bin/echo -n $a | sed s/3/4/`
    sed -f 01tmp/$a-sort.sed 01tmp/$a.xml | \
	xsltproc $libscripts/p3-otlsel-p4.xsl - >01tmp/$p4fn.xml
    tabres=$pubdir/$p4fn.tab
    xmlres=$xmldir/$p4fn.xml
    mv 01tmp/$p4fn.xml $xmlres ; chmod o+r $xmlres
    mv 01tmp/$a.tab $tabres ; chmod o+r $tabres
done
