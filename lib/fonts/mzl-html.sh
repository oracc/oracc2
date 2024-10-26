#!/bin/sh
cut -f2 Oracc-Assyrian.data | sed 's/^ \+//' | paste mzl.lst - oracc-assyrian-utf8.lst >font.tab
rocox -R '<tr><td>%1</td><td>%2</td><td>%3</td></tr>' <font.tab
