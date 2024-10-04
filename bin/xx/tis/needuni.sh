#!/bin/sh
grep uchar  $ORACC/osl/02pub/sl/sl.tsv | grep -v X | sed 's/;uchar//' >have
cut -f1 need | sort -u >need.1
cut -f1 have | sort -u >have.1
comm -23 need.1 have.1
