#!/bin/dash
echo $0 $*
l=$1
p=$2
if [ "$p" != "" ]; then
    p="$p/"
fi
for a in `tr -s ' \r' '\n\n' <00lib/$1.lst` ; do
    lx-atf.sh $p$a
done
