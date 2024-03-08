#!/bin/dash
# $1=project $2=input $3=output
wm -p $1 -l -i $2 | ce2 -3 -p $1 -l >$3
