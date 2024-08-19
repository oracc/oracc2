#!/bin/sh
echo $0 in `pwd`
xsltproc $ORACC/lib/scripts/sxweb-oid-tab.xsl 02xml/sl.xml | \
    sort | sed 's/u[12]/u/' >signlist/02pub/oid.tsv
(cd signlist/02pub ; dbix -d . -n oid oid.tsv ; chmod -R o+r *)
