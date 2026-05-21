#!/bin/sh
#
# pass output of tokx via stdin and write 4-col output to stdout
#
# TOKENID TOKEN COUNT PERCENT
#
p=`oraccopt`
tokexg -s $l -p$p | tee 02pub/g.tkx | tokix -p$p | \
    tee 02pub/g.tix | grep -v : | tokpctg
