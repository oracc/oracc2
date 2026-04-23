#!/bin/sh
#
# This script assumes that cx-marshall.sh has previously been run
# earlier in the odo build process.  It is allowed for 01bld/cat to
# be empty at this point--this just means there is no local catalogue
# data.
#
#set -x
echo $0 $*
set -a
bin="$ORACC/bin"
projtype=`oraccopt . type`;
ldir=01tmp/00cat/l
mdir=01tmp/00cat/m
if [ ${projtype} = "ood" ]; then
    ${bin}/cx 02pub/data.xml
else
    ${bin}/cx-policy.sh

    # set up $mdir/local-[pqx].tsv
    ${bin}/cx-local.sh

    # process 00lib/cat.d into $mdir/extra-[pqx].tsv
    ${bin}/cx-extra.sh

    if [ -s 01bld/lists/proxy-cat.lst ]; then
	${bin}/cx-outer.sh 01bld/lists/proxy-cat.lst
    fi
    m=01tmp/00cat/m
    ${bin}/cx-clean.sh $m/*.tsv
    set $m/*.tsv
    proj=`oraccopt`
    if [ "$*" != "$m/*.tsv" ]; then
	if [ "$2" = "" ]; then
	    cp $1 01bld/cat/union.tsv
	else
	    set $m/extra-[pqx].tsv
	    if [ "$1" == "$m/extra-[pqx].tsv" ]; then
		catextra=""
	    else
		catextra=$*
	    fi
	    set $m/local-[pqx].tsv
	    if [ "$1" == "$m/local-[pqx].tsv" ]; then
		catlocal=""
	    else
		catlocal=$*
	    fi
	    set $m/outer-[pqx].tsv
	    if [ "$1" == "$m/outer-[pqx].tsv" ]; then
		catouter=""
	    else
		catouter=$*
	    fi
	    cx-union.sh $catouter $catlocal $catextra
	fi
	if [ "$catlocal$catouter$catextra" != "" ]; then
	    ${bin}/cx -p$proj 01bld/cat/union.tsv | \
		tee 01bld/cdlicat.xmd | ${bin}/xmlsplit
	    if [ -r 01bld/sortinfo.tab ]; then
		${bin}/pgcsix 01bld/sortinfo.tab && mv 01bld/sortinfo.csi 02pub/
	    fi
	else
	    echo $0: no .tsv in $m
	    exit 1
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
