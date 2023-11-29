#!/bin/dash
echo $0 $*
(cd .. ;
 find $1/00* -type f | xargs ls -l --time-style=long-iso)
