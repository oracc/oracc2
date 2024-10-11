#!/bin/dash
#
# Perform build for a signlist project.
#
# odo always does an odo-update so there's no need to create the
# indices etc. here.
#
echo $0 $*
projtype=`oraccopt . type`
asl=""
if [ "$projtype" = "corpus" ]; then
    if [ "$aslauto" = "yes" ]; then
	sx-csl.sh $tok
	sx-slix.sh $project 01tmp/csl.asl
	asl="01tmp/csl.asl"
    fi
else
    true # need to make stats for osl/pcsl
    asl="static"
fi
if [ "$asl" != "" ]; then
    sxweb.sh
    tisdbi.sh
    if [ "$projtype" = "signlist" ]; then
	odo-portal.sh
    fi
fi
