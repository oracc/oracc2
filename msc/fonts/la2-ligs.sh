#!/bin/sh
#
# Extract a reasonable set of la2-N entries from .atf files; some may
# fail but this produces an XML file with one entry per <l> node
# suitable for use with la2-ttx.xsl
#
if [ "$*" == "" ]; then
    echo $0: must give .atf file(s) on command line. Stop.
    exit 1
fi
grep -A1 ' la₂' $* | grep -B1 lal.less | grep ' la₂' >la2.grep
sed 's/^.* la₂/la₂/' la2.grep | cut -d' ' -f2 | tr -d '[]#?*!' | sed 's/-.*$//' | \
    sort -u | grep ^[0-9] >la2.ligs
gdlx -s <la2.ligs | cut -f2 | grep -f - /home/oracc/osl/02pub/unicode.tsv  | \
    cut -f1 | sed 's/U+/u/' >la2.ulig
/bin/echo '<ll>'`sed 's#^\(.*\)$#<l>\1</l>#' <la2.ulig`'</ll>' | tr -d ' \n' >ll.xml
