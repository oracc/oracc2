#!/bin/sh
#
# Create list of missing/needed signs
#
# Invoke as csl-miss-need.sh miss to generate page of signs in corpus but not in CSL
#
# Invoke as csl-miss-need.sh need to generate page of signs in corpus but not in Unicode
#
echo $0 $*
mode=$1

if [ "$mode" = "miss" ]; then
    missneedsh=cslneedsign.sh
else
    missneedsh=crpneeduni.sh
fi

p=`oraccopt`

echo $0 working in mode=$mode in project $p using script $missneedsh

mkdir -p 00etc

$missneedsh $p | \
    rocox -R '<oid xml:id="%1" freq="%2"/>' -x oids | \
    xsltproc -stringparam slxml \
	     $ORACC/osl/02xml/sl.xml $ORACC/lib/scripts/sx-add-sort.xsl - \
    >00etc/csl-$mode.xml
n=`xmllint --format 00etc/csl-$mode.xml | grep -c '<oid '`
if [ "$n" != "1" ]; then
    s="s"
    isare="are"
else
    isare="is"
fi
echo 's/@@ISARE@@/'$isare'/g' >csl.sed
echo 's/@@NEEDED@@/'$n'/' >>csl.sed
echo 's/@@s@@/'$s'/' >>csl.sed
sed -f csl.sed <$ORACC/lib/data/signlist-csl-$mode-pos.xml >00etc/csl-$mode-pos.xml
rm -f need have need.1 have.1 csl.sed
