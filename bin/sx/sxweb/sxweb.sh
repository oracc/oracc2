#!/bin/sh

function sxinst {
    if [ -r 00lib/$1 ]; then
	input=00lib/$1
    else
	input=$libdata/$1
    fi
    echo sxinst $input
    cat >01tmp/slconfig.sed <<EOF
s#@@PROJECT@@#$abbrev#g
s#@@project@@#$project#g
s#@@hproject@@#$hproject#g
s#@@ASLSCRIPT@@#$aslscript#g
EOF
    sed -f 01tmp/slconfig.sed $input >$2
}

project=`oraccopt`

if [ "$project" == "" ]; then
    echo "$0: must be called in a project directory. Stop."
    exit 1
fi

auto=`oraccopt . asl-auto`
stats=`oraccopt . asl-stats`

if [ "$auto" == "yes" ]; then
    asl=01tmp/csl.asl
    if [ ! -r $asl ]; then
	echo "$0: no csl.asl signlist in 01tmp. Stop."
	exit 1
    fi
    if [ "$stats" != "no" ]; then
	stats="yes"
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

if [ "$auto" == "yes" ]; then
    echo sxweb.sh: auto.asl project=$project hproject=$hproject abbrev=$abbrev
fi

libdata=$ORACC_BUILDS/lib/data

ls -l 02xml/sl.xml

# FIXME: probably need project-config-level control of this
if [ "$auto" != "yes" ]; then
    sxmissing.sh 00etc $asl
    sxudata.sh 00etc $asl
    sxportal.sh
fi

if [ "$stats" == "yes" ]; then
#    csl-need.sh
    csl-miss-need.sh miss
    csl-miss-need.sh need
    ofp=`oraccopt . asl-ofp`
    if [ "$ofp" != "" ]; then
	
    fi
fi
sxweb-pos.sh

sxweb-lists.sh

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

if [ -r 00lib/signlist-config.xml ]; then
    cp -a 00lib/signlist-config.xml signlist/00lib/config.xml
else
    aslscript=`oraccopt . asl-script`
    if [ "$aslscript" == "" ]; then
	aslscript="default"
    fi
    sxinst signlist-config.xml signlist/00lib/config.xml
fi
cp -a signlist/00lib/config.xml signlist/02www
cp -a signlist/00lib/config.xml signlist/02xml
sxinst signlist-parameters.xml signlist/00web/00config/parameters.xml
if [ -r 00lib/version.txt ]; then
    version=`cat 00lib/version.txt`
fi
timestamp=`isogmt -f`
cat >01tmp/verstime.sed <<EOF
s/@@version@@/$version/
s/@@timestamp@@/$timestamp/
EOF
sed -in -f 01tmp/verstime.sed signlist/00web/00config/parameters.xml
sxinst signlist-index.xml signlist/00web/index.xml
sxinst signlist-projesp.css signlist/00res/css/projesp.css
sxinst signlist-projesp.js signlist/00res/js/projesp.js
sxinst signlist-slpage.js signlist/00res/js/slpage.js

set 00lib/signlist-x-*.xml
if [ "$1" != "00lib/signlist-x-*.xml" ] ; then
    cp -f 00lib/signlist-x-*.xml signlist/00web
fi

set 00lib/signlist-x-*.css
if [ "$1" != "00lib/signlist-x-*.css" ] ; then
    cp -f 00lib/signlist-x-*.css signlist/00res/css
fi

if [ "$stats" == "yes" ]; then
    sxfreq.sh
fi

sxsortorder.sh $project

libscripts=$ORACC_BUILDS/lib/scripts

sxweb-oid.sh

xmllint --xinclude - < $libscripts/sxweb-structure.xsl >01tmp/sxweb-structure-xi.xsl

xsltproc 01tmp/sxweb-structure-xi.xsl 02xml/sl.xml >signlist/00web/00config/structure.xml

xsltproc -stringparam project $project $libscripts/sxweb-letters.xsl 02xml/sl.xml

if [ "$abbrev" != "osl" ]; then
    xsltproc $libscripts/sxweb-atoms.xsl 02xml/sl.xml \
	| xsltproc -stringparam project $project $libscripts/sxweb-overview.xsl - >signlist/00web/overview.xml
fi

if [ "$project" = "osl" ]; then
    cp 00etc/Oracc_OSL.txt 02pub
    chmod o+r 02pub/Oracc_OSL.txt
fi

xsltproc -stringparam project $project -stringparam projesp "$ORACC_BUILDS/$project" $libscripts/sxweb-signs.xsl 02xml/sl.xml

if [ -r 00etc/signlist-periods.tab ]; then
    cat 00etc/signlist-periods.tab | while IFS=$'\t' read index title
    do
	  xsltproc -stringparam title "$title" -stringparam index $index \
		   $libscripts/sxweb-period.xsl 02xml/sl-corpus-counts.xml \
		   >signlist/00web/sx-period-$index.xml
    done
fi

(cd signlist ; rm -fr 02www/selpages ; mv 01bld/selpages 02www ; chmod -R o+r 02www/selpages)
(cd signlist ; o2-portal.sh)
(cd signlist ; sl-jsmap.sh ../02xml/sl.xml >02www/js/slmap.js ; chmod o+r 02www/js/slmap.js)
(cp 02xml/sl.xml 02pub/sl.json 02www/downloads ; chmod o+r 02www/downloads)
