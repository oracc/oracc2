#!/bin/dash
# $1=project $2=input $3=output
wm -p $1 -l -i $2 | ce_xtf -3 -p $1 -l >$3
