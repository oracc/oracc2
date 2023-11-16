#!/bin/sh
echo '### RUNNING pctc-update.sh ###'
sx -c 00lib/pcsl.asl
if [ $? == 0 ]; then
    sx -s 00lib/pcsl.asl >02pub/sl/sl.tsv
    slix 02pub/sl/sl.tsv
    chmod o+r 02pub/sl/*
    echo "pctc-update.sh: pctc data updated; web portal unchanged."
else
    echo "pctc-update.sh: errors in pctc processing. Stop."
    exit 1
fi
