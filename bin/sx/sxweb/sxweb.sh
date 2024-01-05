#!/bin/sh

project=`oraccopt`

if [ "$project" == "" ]; then
    echo "$0: must be called in a project directory. Stop."
    exit 1
fi

stats=$*

auto=`oraccopt . asl-auto`

if [ "$auto" == "yes" ]; then
    asl=01tmp/auto.asl
    if [ ! -r $asl ]; then
	echo "$0: no auto.asl signlist in 01tmp. Stop."
	exit 1
    else
	mcu-slix.sh $project $asl
    fi
else
    set 00lib/*.asl
    if [ "$1" == "00lib/*.asl" ]; then
	echo "$0: no .asl signlist in 00lib. Stop."
	exit 1
    else
	asl=$1
    fi
fi

abbrev=`oraccopt . abbrev`
hproject=`/bin/echo -n $project | tr / -`
libdata=$ORACC_BUILDS/lib/data

function sxinst {    
    sed "s/@@PROJECT@@/$abbrev/g" $libdata/$1 \
	| sed "s/@@project@@/$project/g" \
	| sed "s/@@hproject@@/$hproject/g" \
	      >$2
}

ls -l 02xml/sl.xml
sxmissing.sh 00etc $asl
sxudata.sh 00etc $asl
sxportal.sh

rm -fr signlist ; mkdir signlist

(cd $ORACC_BUILDS ;
    mkdir -p bld/$project/signlist
    for a in xml www pub ; do
	mkdir -p $a/$project/signlist
    done
)

(cd signlist ;
    mkdir -p 00lib ;
    mkdir -p 00res/css ;
    mkdir -p 00res/js ;
    mkdir -p 00web/00config ;
    mkdir -p 01tmp ;
    ln -sf $ORACC_BUILDS/bld/$project/signlist 01bld
    for a in xml www pub ; do
	ln -sf $ORACC_BUILDS/$a/$project/signlist 02$a
    done
    mkdir -p 01bld/www
)

sxinst signlist-config.xml signlist/00lib/config.xml
sxinst signlist-index.html signlist/00lib/signlist-index.html
sxinst signlist-parameters.xml signlist/00web/00config/parameters.xml
sxinst signlist-home.xml signlist/00web/home.xml
sxinst signlist-slform.html signlist/00web/slform.html
sxinst signlist-projesp.css signlist/00res/css/projesp.css
sxinst signlist-sl.css signlist/00res/css/sl.css
sxinst signlist-projesp.js signlist/00res/js/projesp.js
sxinst signlist-sl.js signlist/00res/js/sl.js

cp -f 00lib/signlist-x-*.xml signlist/00web
cp -f 00lib/signlist-x-*.css signlist/00res/css

libscripts=$ORACC_BUILDS/lib/scripts

xsltproc $libscripts/sxweb-oid-tab.xsl 02xml/sl.xml >signlist/02pub/oid-index.tab
mkdir -p signlist/02pub/oid
dbix -d signlist/02pub/oid -n oid signlist/02pub/oid-index.tab
chmod -R o+r signlist/02pub

xmllint --xinclude - < $libscripts/sxweb-structure.xsl >01tmp/sxweb-structure-xi.xsl

xsltproc 01tmp/sxweb-structure-xi.xsl 02xml/sl.xml >signlist/00web/00config/structure.xml

xsltproc $libscripts/sxweb-letters.xsl 02xml/sl.xml

if [ "$abbrev" != "ogsl" ]; then
    xsltproc $libscripts/sxweb-atoms.xsl 02xml/sl.xml \
	| xsltproc $libscripts/sxweb-overview.xsl - >signlist/00web/overview.xml
fi

if [ "$stats" = "with-stats" ]; then
    echo with-stats=true
    xsltproc -stringparam with-stats yes -stringparam project $project $libscripts/sxweb-signs.xsl 02xml/sl.xml
else 
    xsltproc -stringparam project $project $libscripts/sxweb-signs.xsl 02xml/sl.xml
fi

if [ -r 00etc/signlist-periods.tab ]; then
    cat 00etc/signlist-periods.tab | while IFS=$'\t' read index title
    do
	  xsltproc -stringparam title "$title" -stringparam index $index \
		   $libscripts/sxweb-period.xsl 02xml/sl-corpus-counts.xml \
		   >signlist/00web/sx-period-$index.xml
    done
fi

(cd signlist ; o2-portal.sh)

if [ -d 01tmp/slinst ]; then
    rm -fr signlist/02www/inst
    mv 01tmp/slinst signlist/02www/inst
    chmod -R o+r signlist/02www/inst
fi

# Adjust home/index so that index.html is the frame that gives
# searchable signlist and home.html is the browsable ESP portal.
## 2023-12-04 esp2 tweaked to write to home.html directly 
if [ -r signlist/00lib/signlist-index.html ]; then
#    (cd signlist/02www ; echo cwd=`pwd` ; mv -v index.html home.html)
    (cd signlist ; echo cwd=`pwd` ; rm -f 02index.html ; \
     cp -vaf 00lib/signlist-index.html 02www/index.html ; \
     chmod o+r 02www/index.html)
fi
