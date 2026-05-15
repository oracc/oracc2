#!/bin/bash

echo $0 $*

project=$1
l=$2
ldir="01bld/$l"
webdir=01bld/www
oracc=$ORACC_BUILDS

mkdir -p $webdir
cp -f 02xml/config.xml $webdir ; chmod o+r $webdir/config.xml

glostyle=`oraccopt . type`
if [ "$glostyle" == "superglo" ]; then
    xcf=l2p3-new-odo.xcf
else
    xcf=l2p3-odo.xcf 
fi

inlinesenses=`oraccopt . cbd-inline-senses`
lexdata=`oraccopt . cbd-lex-data`
cbdfiles=`oraccopt . cbd-use-files`

echo '>>>'using configuration file $xcf

rm -fr $webdir/cbd/$l ; mkdir -p $webdir/cbd/$l
g2c="$ldir/$l.g2c"
if [ -r $g2c ]; then
    # needs to happen before l2-glomanager 
    if [ "$cbdfiles" == "yes" ]; then
	echo '>>>'editing file tags into xincludes
	cbdfile.plx -w 02www -l $l
    fi
    echo '>>>'producing web version of $l
    #	    echo ">>>l2-glomanager -webdir=$webdir -conf $xcf -cbdlang $l"
    l2-glomanager.plx -webdir=$webdir -conf $xcf -cbdlang $l
    #	    echo g2c-sig-map
    xsltproc $ORACC/lib/scripts/g2c-sig-map.xsl 01bld/$l/articles.xml >$webdir/cbd/$l/$l.map
	    
    #	    echo xff
    xfftab=`oraccopt . cbd-forms-table`
    if [ "$xfftab" = 'yes' ]; then
	l2p3-xff.sh $webdir $l
    fi
    if [ "$inlinesenses" == "yes" ]; then
	l2p3-inline-senses.sh $webdir $l
    fi
    if [ "$lexdata" == "yes" ]; then
	lex-xincludes.plx 02www/cbd $l 2>lex-includes.log
    fi	    
fi

# Index the glossary
mkdir -p $oracc/pub/$project/cbd/$l
$oracc/bin/secbdx -p $project -l $l 01bld/$l/articles.xml
if [ "$l" = "sux" ]; then
    $oracc/bin/lex-tis.sh
fi
$oracc/bin/xisdb $oracc/pub/$project/cbd/$l/$l.tis
chmod -R o+r $oracc/pub/$project/cbd/$l
