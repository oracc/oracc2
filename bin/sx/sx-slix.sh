#!/bin/dash
#
# sx-slix.sh updates the signlist data for a project creating the
# index in 02pub/sl and the XML version in 02xml/sl.xml.
#
echo $0 $*
project=$1
asl=$2

asldomain=`oraccopt . asl-domain`
if [ "$asldomain" = "" ]; then
    asldomain=sl
fi

aslcbdtok=`oraccopt . asl-cbd-tok`
if [ "$aslcbdtok" != "" ]; then
    echo asl-cbd-tok=$aslcbdtok
    aslcbdurl=`oraccopt . asl-cbd-url`
    Larg="-L$aslcbdtok"
    Lurl="-l$aslcbdurl"
elif [ -r 01tmp/cbd.tok ]; then
    echo asl-cbd-tok not set, found 01tmp/cbd.tok
    Larg="-L01tmp/cbd.tok"
fi

mkdir -p 02pub/sl
if [ "$asl" = "" ]; then
    set 00lib/*.asl
    if [ "$1" != "00lib/*.asl" ]; then
	if [ "$2" != "" ]; then
	    echo $0: a project can only have one .asl file in 00lib. Stop.
	    exit 1
	fi
	asl=$1
    else
	if [ -r 01tmp/csl.asl ]; then
	    asl=01tmp/csl.asl
	fi	
    fi
fi
if [ "$asl" != "" ]; then
    echo $0: updating $asl in `pwd`
    # check signlist
    sx -c $asl
    if [ $? -eq 0 ]; then
	sx -s $asl >02pub/sl/sl.tsv
	slix 02pub/sl/sl.tsv
	# This needs to be configurable so that OSL could find $ORACC/cdli/01tmp/csl.kis, e.g.
	if [ -r 01tmp/csl.kis ]; then
	    tis=-I$asldomain:01tmp/csl.kis
	fi
	echo "$0: sx -dscripts,LISTS $tis $Larg $Lurl -X 02xml/sl.xml -J 02pub/sl.json $asl"
	sx -dscripts,LISTS $tis $Larg $Lurl -X 02xml/sl.xml -J 02pub/sl.json $asl
	mv -f sx-lists.out 02pub/lists.tsv
	if [ -r 02xml/csldist.xml ]; then
	    xsltproc -o 02xml/sl.xml $ORACC/lib/scripts/csldist.xsl 02xml/sl.xml
	fi
	# cp 02xml/sl.xml 02xml/sl-mcu.xml
	sx -S $asl | tee 02pub/sortcodes.tsv | \
	    rocox -R '<t c="%2">%1</t>' -x sort >02pub/sortcodes.xml
	sx -u $asl
	cut -f 2-4,6 unicode.tsv | grep '	o[0-9]\+	' >02pub/unicode.tsv
	rm -f unicode.tsv
	x=$?
	chmod -f -R o+r 02pub/sl 02pub/lists.tsv 02pub/sortcodes.* 02pub/unicode.tsv 02xml/sl.xml 02pub/*.oss 02pub/*.mrg
	# ignore chmod status because MacOS -f doesn't work
	exit $x
    else
	echo "$0: errors in processing $asl. Stop."
	exit 1
    fi
else
    aslauto=`oraccopt . asl-auto`
    if [ "$aslauto" = "yes" ]; then
	if [ -r 01bld/buildstamp ]; then
	    echo "$0: no .asl found but project has been built."
	    echo "$0: do 'rm -f `pwd`/01bld/buildstamp' and try again."
	    exit 1
	else
	    echo "$0: igoring lack of .asl because project has not been built."
	    exit 1
	fi
    else
	echo "$0: no 00lib/*.asl and asl-auto != 'yes'."
	exit 0
    fi
fi
