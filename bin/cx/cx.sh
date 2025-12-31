#!/bin/sh
echo $0 $*
bin="$ORACC/bin"
projtype=`oraccopt . type`;
if [ ${projtype} = "ood" ]; then
    ${bin}/cx 02pub/data.xml
else
    set 00cat/*.tsv
    if [ "$1" != "00cat/*.tsv" ]; then
	${bin}/cx $1
    else
	echo $0: no .tsv file found in 00cat. Stop.
	exit 1
    fi
fi
