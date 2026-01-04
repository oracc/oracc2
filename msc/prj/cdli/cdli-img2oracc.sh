#!/bin/sh
bin=$ORACC/bin
pubd=$ORACC/cdli/02pub
imgd=$pubd/img
mkdir -p $imgd
csvformat -T imgs.csv | cut -f1,2 | \
    tr '\t' '\n' | grep P[0-9] | $bin/imgp4 >$pubd/img.tsv
chmod -R o+r $pubd
cat $pubd/img.tsv | $bin/dbix -fs -d $imgd -n img
