#!/bin/sh
sedf=$1
in=$2;
out=`basename $in .in`
rm -f $out
sed -f $sedf <$in >$out
chmod -w $out
if [[ $out == *.sh ]]; then
    echo $0 chown +x $out
    chmod +x $out
fi
