#!/bin/sh
sudo -E mkdir -p 
csvformat -T imgs.csv | cut -f1,2 | \
    tr '\t' '\n' | grep P[0-9] | imgp4 | dbix -s -d $ORACC/cdli/02pub/img -n img
