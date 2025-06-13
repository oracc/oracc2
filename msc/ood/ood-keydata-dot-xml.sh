#!/bin/sh
if [ -r 00lib/keydata.xml ]; then
    b=`basename $0`
    echo "$0: 00lib/keydata.xml"
    echo "already exists. To create it anew you need to do:"
    echo
    echo "  rm 00lib/keydata.xml"
    echo
    echo "and then run $b again."
    exit 1
fi
ff=`xmllint --xpath '*/*/*' 00lib/data.xml | grep '^<' | cut -d'>' -f1 | tr -d '</' | sort -u`
exec 3>&1
exec >00lib/keydata.xml
echo '<?xml version="1.0" encoding="utf-8"?>'
echo '<keydata xmlns="http://oracc.org/ns/keydata/1.0">'
echo '  <not-indexed/>'
echo '  <sortable>'
for f in $ff; do
    echo "    <field n=\"$f\"	hr=\"$f\"/>"
done
echo '  </sortable>'
for f in $ff; do
    echo "  <keys field=\"$f\" reorder=\"1\" method=\"string\" class=\"open\"/>"
done
echo '</keydata>'
exec 1>&3
echo "00lib/keydata.xml has been created"
