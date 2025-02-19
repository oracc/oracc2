#!/bin/sh
#
# pass output of tokx via stdin and write 4-col output to stdout
#
# TOKENID TOKEN COUNT PERCENT
#
p=`oraccopt`
if [ -r 00lib/toklists.lst ]; then
    l="-l00lib/toklists.lst"
fi
echo $0 using l-arg "'$l'"
tokex $l -p$p | tee 02pub/csl.tkx | tokix -p$p | tee 02pub/csl.tix | grep -v : | tokpct
