#!/bin/dash
#
# mcu-slix.sh updates the signlist data for a project creating the
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
echo asl-cbd-tok=$aslcbdtok
if [ "$aslcbdtok" != "" ]; then
    aslcbdurl=`oraccopt . asl-cbd-url`
    Larg="-L$aslcbdtok"
    Lurl="-l$aslcbdurl"
elif [ -r 01tmp/cbd.tok ]; then
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
    fi
fi
if [ "$asl" = "01tmp/auto.asl" ]; then
    if [ "$asldomain" = "sl" ]; then
	UP=-Upogsl
    elif [ "$asldomain" = "pc" ]; then
	UP=-Uppctc
    fi
fi
if [ "$asl" != "" ]; then
    echo $0: updating $asl in `pwd`
    # check signlist
    sx $UP -c $asl
    if [ $? -eq 0 ]; then
	sx $UP -s $asl >02pub/sl/sl.tsv
	slix 02pub/sl/sl.tsv
	if [ -r 01tmp/g.tis ]; then
	    tis=-I$asldomain:01tmp/g.tis
	fi
	echo "$0: sx $UP $tis $Larg $Lurl -X 02xml/sl.xml $asl"
	sx $UP $tis $Larg $Lurl -X 02xml/sl.xml $asl
	cp 02xml/sl.xml 02xml/sl-mcu.xml
	sx $UP -S $asl | tee 02pub/sortcodes.tsv | \
	    rocox -R '<t c="%2">%1</t>' -x sort >02pub/sortcodes.xml
	chmod -R o+r 02pub/sl 02pub/sortcodes.* 02xml/sl.xml
    else
	echo "$0: errors in processing $asl. Stop."
	exit 1
    fi
fi
