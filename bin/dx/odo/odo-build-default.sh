#!/bin/dash
echo $0 $*
o2-clean-build.sh $subcommand 
o2-web.sh
