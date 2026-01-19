#!/bin/sh
#
# This script assumes that cx-marshall.sh has previously been run
# earlier in the odo build process.  It is allowed for 01bld/00cat to
# be empty at this point--this just means there is no local catalogue
# data.
#
echo $0 $*
bin="$ORACC/bin"
projtype=`oraccopt . type`;
if [ ${projtype} = "ood" ]; then
    ${bin}/cx 02pub/data.xml
else
    ${bin}/cx-policy.sh
    if [ -s 01bld/lists/proxy-cat.lst ]; then
	${bin}/cx-outer.sh 01bld/lists/proxy-cat.lst
    fi
    ${bin}/cx-clean.sh 01bld/00cat/*.tsv
    set 01bld/00cat/*.tsv
    if [ "$*" != "01bld/00cat/*.tsv" ]; then
	${bin}/cx 01bld/00cat/local-[pqx].tsv 01bld/00cat/outer-[pqx].tsv
	if [ -r 01bld/sortinfo.tab ]; then
	    ${bin}/pgcsix 01bld/sortinfo.tab
	fi
    else
	if [ -s 01bld/lists/approved.lst ]; then
	    echo $0: no metadata found for text data. Stop.
	    exit 1
	else
	    echo $0: no catalogue files, no metadata processing.
	fi
    fi
fi
