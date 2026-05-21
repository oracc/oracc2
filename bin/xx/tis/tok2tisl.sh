#!/bin/sh
#
# pass output of tokx via stdin and write 4-col output to stdout
#
# TOKENID TOKEN COUNT PERCENT
#
p=`oraccopt`
tokexl -s $l -p$p | tee 02pub/l.tkx | tokix -p$p | \
    tee 02pub/l.tix | tokpctl
