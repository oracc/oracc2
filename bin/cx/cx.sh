#!/bin/sh
echo $0 $*
bin="$ORACC/bin"
projtype=`oraccopt . type`;
if [ ${projtype} = "ood" ]; then
    ${bin}/cx 02pub/data.xml
else
    set 01tmp/00cat/*.tsv
    if [ "$1" != "00cat/*.tsv" ]; then
	${bin}/cx-marshall.sh
	${bin}/cx-extra.sh
	${bin}/cx-all.sh
	${bin}/cx 01tmp/00cat/all
	if [ -r 01bld/sortinfo.tab ]; then
	    ${bin}/pgcsix 01bld/sortinfo.tab
	fi
    else
	echo $0: no .tsv file found in 00cat. Stop.
	exit 1
    fi
fi
