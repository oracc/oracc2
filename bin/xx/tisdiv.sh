#!/bin/dash
project=$1
shift
/bin/echo $* | tr ' ' '\n' | wm -l -p $project | ce_xtf -3 -p $project -l | xis-xml-html.sh $project
