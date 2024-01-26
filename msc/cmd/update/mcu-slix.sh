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
if [ "$asl" != "" ]; then
    echo $0: updating $asl
    # check signlist
    sx -c $asl
    if [ $? -eq 0 ]; then
	sx -s $asl >02pub/sl/sl.tsv
	slix 02pub/sl/sl.tsv
	if [ -r 01tmp/g.tis ]; then
	    tis=-I$asldomain:01tmp/g.tis
	fi
	echo $0: tis=$tis
	sx $tis -X 02xml/sl.xml $asl
	sx -S $asl | tee 02pub/sortcodes.tsv | \
	    rocox -R '<t c="%2">%1</t>' -x sort >02pub/sortcodes.xml
	chmod -R o+r 02pub/sl 02pub/sortcodes.* 02xml/sl.xml
    else
	echo "$0: errors in processing $asl. Stop."
	exit 1
    fi
fi
