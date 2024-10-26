#!/bin/sh
imbase=`grep @css $1.def | cut -f2`
cut -f11-12 $1.ofp | rocox -C21 | grep ^o | sed 's/\t/\t\t/' | cat $1.def - >im-$imbase.tab
