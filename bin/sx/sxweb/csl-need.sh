#!/bin/sh
#
# Create list of needed signs
p=`oraccopt`
crpneeduni.sh $p | \
    rocox -R '<oid xml:id="%1" freq="%2"/>' -x oids | \
    xsltproc -stringparam slxml \
	     $ORACC/osl/02xml/sl.xml $ORACC/lib/scripts/sx-add-sort.xsl - \
    >00etc/csl-need.xml
n=`xmllint --format 00etc/csl-need.xml | grep -c '<oid '`
if [ "$n" != "0" ]; then
    if [ "$n" != "1" ]; then
	s="s"
    fi
    echo 's/@@NEEDED@@/'$n'/' >csl.sed
    echo 's/@@s@@/'$s'/' >>csl.sed
    sed -f csl.sed <$ORACC/lib/data/signlist-csl-need-pos.xml >00etc/csl-need-pos.xml
fi
rm -f need have need.1 have.1 csl.sed
