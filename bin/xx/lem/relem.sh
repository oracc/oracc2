#!/bin/sh
xtf=$1
wid2lem -s <$xtf  | grep -v '	[0-9]\+$' | cut -f1,3 | sed 's#@.*//##' | \
    sed 's/\].*$//' | sed 's/BAD$/u/' >rl_orig.tsv
wid2err -s <$xtf >rl_wids.tsv
