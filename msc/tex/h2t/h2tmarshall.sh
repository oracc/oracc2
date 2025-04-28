#!/bin/sh
base=/Users/stinney/orc/www/pcsl/
rm -fr h2t-files ; mkdir -p h2t-files
for a in `grep ^+ h2t-esp.url | cut -f2 | cat - h2t-add.url`; do
    xl $base/$a | grep -v '^<!DOCTYP' >$base/$a.xml
done
