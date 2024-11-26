#!/bin/sh
#
# Build the 02pub/oid.tsv list and index it.
#
# If a project is a signlist this is already built for us by the sxweb
# scripts.
#
# If a project has a signlist subproject, we include it in the parent
# project oid.tsv.
#
# For glossary entries, this script depends on 02pub/cbd/oxid.tsv
# which is created by cbd-items.sh. If an OID occurs in oxid.tsv in
# more than one glossary the list is purged of qpn-x-* entries because
# they are always a subset of qpn.
#
# For non-qpn, if the config option cbd-unify is 'yes' they
# sub-glossary items are also purged.
#
# After purging, any glossary entries that still occur in more than
# one glossary are not added to oid.tsv--they are handled later by
# px's oxid db.
#
# The structure of oid.tsv is:
#
# OID\t[uh]\tURL
#
# For 'u' entries a 302 is emitted with the URL.
#
# For 'h' entries, the HTML file is returned directly.
#

p=$1
if [ "$p" == "" ]; then
    p=`oraccopt`
    if [ "$p" == "" ]; then
	echo $0: no project on command line or config. Stop.
	exit 1
    fi
fi

b=`basename $p`
if [ "$b" == "signlist" ]; then
    echo $0: project is a signlist so our work is done here. Stop.
    exit 0
fi

puboid=02pub/oid.tsv

rm -f $puboid

if [ -s signlist/02pub/oid.tsv ]; then
    cp signlist/02pub/oid.tsv $puboid
    if [ "$p" == "osl" ]; then
	if [ -s ../pcsl/02pub/oid.tsv ]; then
	    cat ../pcsl/02pub/oid.tsv >>$puboid
	    rm -f signlist/02pub/oid.{dbh,dbi}
	fi
	# could add other sl here also
    fi
elif [ ! -s 02pub/cbd/oxid.tsv ]; then
    echo $0: no signlist and no glossary OIDs. Nothing to do here. Stop.
    exit 0
fi

if [ -s 02pub/cbd/oxid.tsv ]; then
    unify=`oraccopt . cbd-unify`
    rocox -C21 <02pub/cbd/oxid.tsv  | concord '-fwcl*st' >oxid.1
    if [ "$unify" == "yes" ]; then
	sed 's/[a-z]\{3\}-x-[a-z]\{6\} //g' <oxid.1 | grep -v ' ' >oxid.2
    else
	sed 's/qpn-x-[a-z]\{6\} //g' <oxid.1 | grep -v ' ' >oxid.2
    fi
    
    rocox -n -R'%1	h	'www/$p/cbd/'%2/%1.html' <oxid.2 >>$puboid
fi

(cd 02pub ; dbix -d . -n oid oid.tsv ; chmod o+r oid.*)
if [ "$p" == "osl" ]; then
    osl=$ORACC/pub/osl
    (cd signlist/02pub ; ln -sf $osl/oid.dbh . ; ln -sf $osl/oid.dbi .)
fi
