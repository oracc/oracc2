#!/bin/sh
#cut -f2 Oracc-Assyrian.data | sed 's/^ \+//' | paste mzl.lst - oracc-assyrian-utf8.lst >font.tab
paste mzl.lst oa-edit.lst >font.tab
cat oa-head.html >oa.html
perl -C -p -e 's#^(.*?)\t(.*?)\t(.*)$#<tr><td>$1</td><td>$2</td><td class="oa">$3</td></tr>#' <font.tab >>oa.html
#rocox -n -R '<tr><td>%1</td><td>%2</td><td class="oa">%3</td></tr>' <font.tab >>oa.html
cat oa-tail.html >>oa.html
