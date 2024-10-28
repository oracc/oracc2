#!/bin/sh
label=`grep @label $1.def | cut -f2`
imbase=`grep @css $1.def | cut -f2`
cut -f11-12 $1.ofp | rocox -C21 | grep ^o | sed 's/$/\t'$label/ | cat $1.def - >im-$imbase.tab
