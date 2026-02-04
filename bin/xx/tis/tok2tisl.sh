#!/bin/sh
#
# pass output of tokx via stdin and write 4-col output to stdout
#
# TOKENID TOKEN COUNT PERCENT
#
p=`oraccopt`
##if [ -r 00lib/toklists.lst ]; then
##    l="-l00lib/toklists.lst"
##fi
tokexl -s $l -p$p | tee 02pub/l.tkx | tokix -p$p | \
    tee 02pub/l.tix | tokpctl
