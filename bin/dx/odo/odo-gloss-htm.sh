#!/bin/bash

echo $0 $*

webdir=01bld/www
mkdir -p $webdir

cp -f 02xml/config.xml $webdir ; chmod o+r $webdir/config.xml

glostyle=`oraccopt . type`
if [ "$glostyle" == "superglo" ]; then
    xcf=l2p3-newstyle.xcf
else
    xcf=l2p3.xcf 
fi

inlinesenses=`oraccopt . cbd-inline-senses`
lexdata=`oraccopt . cbd-lex-data`
cbdfiles=`oraccopt . cbd-use-files`

echo '>>>'using configuration file $xcf

rm -fr $webdir/cbd ; mkdir -p $webdir/cbd
g2c=`find 01bld/* -maxdepth 1 -name '*.g2c'`
if [ "$g2c" != "" ]; then
    for g2c in `ls 01bld/*/*.g2c` ; do
	# -r fails if the glossary is empty because it points to *.cbd,
	# which doesn't exist
	if [ -r $g2c ]; then
	    ldir=`dirname $g2c`
	    l=`basename $ldir`
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
    done
fi

# Do this late so that all the necessary support files have been built
odo-lexdata.sh

jumps=`oraccopt . outline-cbd-jumps`
if [ "$jumps" == "yes" ]; then
    ns-art-outlines.sh
fi
