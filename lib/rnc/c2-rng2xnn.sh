#!/bin/sh
rm -f rngnames.out
for a in *.rng; do
    ../../utl/xl $a | grep '\( ns=\|xmlns:\)' | \
	tr ' ' '\n' | tr -d '>"' | \
	grep -v '^<' >> rngnames.out
    ../../utl/xl $a | grep '<attribute name=\|<element name=' | \
	sed 's/ name=/=/g' | \
	tr -d '<>"/' | tr -s ' ' | sed 's/^ //' >>rngnames.out
done
grep -v xmlns= rngnames.out >$1


# /opt/homebrew/Cellar/pcre2/10.45/include/
