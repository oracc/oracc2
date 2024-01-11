#!/bin/dash
project=$1
shift
divfile=$1
shift
echo $* | tr ' ' '\n' | \
    wm -l -p $project | ce_xtf -3 -p $project -l | \
    xis-xml-html.sh $project >>$divfile
