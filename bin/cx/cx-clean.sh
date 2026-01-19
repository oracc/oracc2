#!/bin/sh
#
# Remove cat files that are empty or only have a fields line
#
for a in $* ; do
    if [ -s $a ]; then
	n=`wc -l $a`
	if [ $n -eq 1 ]; then
	    rm -f $a
	fi
    else
	rm -f $a
    fi
done
