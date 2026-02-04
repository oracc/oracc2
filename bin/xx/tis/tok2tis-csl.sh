#!/bin/sh
#
# Pass output of tokx via stdin and write 4-col output to stdout
#
# This version of the pipeline is for corpus-sign-lists (csl)
#
# TOKENID TOKEN COUNT PERCENT
#
p=`oraccopt`
if [ -r 00lib/toklists.lst ]; then
    l="-l00lib/toklists.lst"
fi
tokexg $l -p$p | tee 02pub/csl.tkx | tokix -p$p | \
    tee 02pub/csl.tix | grep -v : | tokpctg
