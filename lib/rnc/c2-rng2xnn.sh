#!/bin/sh
top=$1
shift
rm -f rngnames.out
for a in $1; do
    $top/utl/xl $a | grep '\( ns=\|xmlns:\)' | \
	tr ' ' '\n' | tr -d '>"' | \
	grep -v '^<' >> rngnames.out
    $top/utl/xl $a | grep '<attribute name=\|<element name=' | \
	sed 's/ name=/=/g' | \
	tr -d '<>"/' | tr -s ' ' | sed 's/^ //' >>rngnames.out
done
grep -v xmlns= rngnames.out >$2
