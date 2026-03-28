#!/bin/sh
#
# This script assumes that cx-marshall.sh has previously been run
# earlier in the odo build process.  It is allowed for 01bld/cat to
# be empty at this point--this just means there is no local catalogue
# data.
#
set -x
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
    ${bin}/cx-clean.sh 01bld/cat/*.tsv
    set 01bld/cat/*.tsv
    proj=`oraccopt`
    if [ "$*" != "01bld/cat/*.tsv" ]; then
	set 01bld/cat/local-[pqx].tsv
	if [ "$1" == "01bld/cat/local-[pqx].tsv" ]; then
	    catlocal=""
	else
	    catlocal=$*
	fi
	set 01bld/cat/outer-[pqx].tsv
	if [ "$1" == "01bld/cat/outer-[pqx].tsv" ]; then
	    catouter=""
	else
	    catouter=$*
	fi
	if [ "$catlocal$catouter" != "" ]; then
	    ${bin}/cx -p$proj $catlocal $catouter | \
		tee 01bld/cdlicat.xmd | ${bin}/xmlsplit
	    if [ -r 01bld/sortinfo.tab ]; then
		${bin}/pgcsix 01bld/sortinfo.tab && mv 01bld/sortinfo.csi 02pub/
	    fi
	else
	    echo $0: no "01bld/cat/local*.tsv or 01bld/cat/outer*.tsv"
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
