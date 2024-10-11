#!/bin/dash
#
# mcu-slix.sh updates the signlist data for a project creating the
# index in 02pub/sl and the XML version in 02xml/sl.xml.
#
echo $0 $*
project=$1
asl=$2

sx-slix.sh $project $asl
