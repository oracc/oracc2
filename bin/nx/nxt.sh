#!/bin/sh
cp nxt.base nxt.g
grep ^@v $ORACC/osl/00lib/osl.asl | grep '[0-9n](' | tr ' ' '\t' \
    | grep '@v	' | cut -f2 | sort -u | sed 's/$/, nxt_ng/'>>nxt.g
