#!/bin/sh
#
# Use 00etc/period+.tsv to make lists of texts for each period.
#
cut -f1,3 00etc/period+.tsv >00etc/period-.tsv
per=`cut -f2 00etc/period-.tsv | sort -u | tr '\n' ' '`
for a in $per ; do
    p=00etc/p-$a.lst
    g="	$a\$"
    echo "making list for $a by grepping '$g' into $p"
    grep "$g" 00etc/period-.tsv | cut -f1 >$p
done
