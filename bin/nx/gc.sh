#!/bin/sh
cp gc.base gc.g
cut -f2 gur-ngm.tsv | sed 's/] /]\n/g' | sort -u | sed 's/$/, nxt_gc/' >>gc.g

