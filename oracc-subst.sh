#!/bin/sh
sedf=$1
in=$2;
out=`basename $in .in`
rm -f $out
sed -f $sedf <$in >$out
chmod -w $out
